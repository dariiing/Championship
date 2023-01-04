#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sqlite3.h>   // baza de date
#include <curl/curl.h> // pentru email


//variabile globale
//int login = 0;   //pt logare
int login_initiated = 0;
int create_initiated = 0;
int participate_initiated = 0;
int editing_initiated = 0;
int pass_initiated = 0;
//int normal = 0; //clienti normali
//int admin = 0; //admini
sqlite3 *db;
int rc;
char *zErrMsg = 0;
char username[30];
char strong[30];
char weak[30];

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

struct infos{
  int login;
  int normal;
  int admin;
}v[1000];

pthread_mutex_t lock;

int search_username(int idThread, char command[])
{
      const char *user;
      sqlite3_stmt *stmt;
      sqlite3_prepare_v2(db,"select username from accounts",-1,&stmt,0);
      while(sqlite3_step(stmt)!=SQLITE_DONE)
	    {
		    user=sqlite3_column_text(stmt,0);
        printf("Verifying every username\n");
		    if(strstr(command,user)!=0) 
		      {
            strcpy(username,command);
            username[strlen(username)-1]='\0';
            printf("Client: %d -> %s found\n",idThread,username);
            strcpy(command,"User found. Enter your password\n");
            return 1;
          }
        
	    }
      rc = sqlite3_finalize(stmt);
      sqlite3_close(db);
      strcpy(command,"Username not found. Try again\n");
      return 0;
}

void search_password(int idThread, char command[])
{
      const char *pass;
      const char *st;
      const char *we;
      const char *type;
      sqlite3_stmt *stmt;
      v[idThread].login = 0;
      v[idThread].normal = 0;
      v[idThread].admin = 0;
      char sql[4000];
      printf("buna siua %s\n",username);
      sprintf(sql,"select password, strong, weak, admin from accounts where username = '%s'",username);
      sqlite3_prepare_v2(db,sql,-1,&stmt,0);
      while(sqlite3_step(stmt)!=SQLITE_DONE)
	    {
		    pass=sqlite3_column_text(stmt,0);
        printf("Verifying the password\n");
		    if(strstr(command,pass)!=0) 
		      {
            printf("Corect password\n");
            printf("Client: %d -> %s found\n",idThread,username);

            st=sqlite3_column_text(stmt,1);
            we=sqlite3_column_text(stmt,2);
            strcpy(strong,st);
            strcpy(weak,we);
            printf("Client: %d -> %s found\n",idThread,strong);
            printf("Client: %d -> %s found\n",idThread,weak);
            v[idThread].login = 1;
            strcpy(command,"Welcome back\n");
            //se verifica tipul de utilizator
            type = (const char *)sqlite3_column_text(stmt, 3);
            int x = atoi(type);
            if(x == 1) { v[idThread].admin = 1; v[idThread].normal = 0; }
            else {v[idThread].normal = 1; v[idThread].admin = 0;}
          }
	    }
      rc = sqlite3_finalize(stmt);
      sqlite3_close(db);
      printf("Client: %d -> Login %d\n", idThread, v[idThread].login);
      printf("Client: %d -> Admin %d\n",idThread, v[idThread].admin);
      printf("Client: %d -> Normal %d\n",idThread, v[idThread].normal);
      if(v[idThread].login == 0) { 
        strcpy(command,"Wrong password. Try again\n");
      }
}

void show_championships()
{
  FILE* fp;
  fp = fopen("championships.txt", "w+");
  fprintf(fp,"\n\n");
  fprintf(fp,"CHAMPIONSHIPS DETAILS                   \n");
  fprintf(fp,"----------------------------------------\n");
  const char *name;
  const char *type;
  const char *nb;
  const char *structure;
  const char *date;
  const char *ora;
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db,"select name, type, nb_players, structure, games,ora  from championships",-1,&stmt,0);
  while(sqlite3_step(stmt)!=SQLITE_DONE)
	    { 
        name=sqlite3_column_text(stmt,0);
        fprintf(fp,"Name: %s\n", name);
        type=sqlite3_column_text(stmt,1);
        fprintf(fp,"Type: %s\n", type);
        nb=sqlite3_column_text(stmt,2);
        fprintf(fp,"Nb of players: %s\n", nb);
        structure=sqlite3_column_text(stmt,3);
        fprintf(fp,"Structure: %s\n", structure);
        date=sqlite3_column_text(stmt,4);
        fprintf(fp,"Date: %s\n", date);
        ora=sqlite3_column_text(stmt,5);
        fprintf(fp,"Hour: %s\n", ora);
        fprintf(fp,"----------------------------------------\n");
	    }
      rc = sqlite3_finalize(stmt);
      sqlite3_close(db);
  fclose(fp);
}

void show_history()
{
  FILE* fp;
  fp = fopen("history.txt", "w+");
  fprintf(fp,"\n\n");
  fprintf(fp,"CHAMPIONSHIPS HISTORY                   \n");
  fprintf(fp,"----------------------------------------\n");
  const char *name;
  const char *history;
  const char *winner;
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db,"select name, history, winner from championships",-1,&stmt,0);
  while(sqlite3_step(stmt)!=SQLITE_DONE)
	    { 
        name=sqlite3_column_text(stmt,0);
        fprintf(fp,"Name: %s\n", name);
        history=sqlite3_column_text(stmt,1);
        fprintf(fp,"History: %s\n", history);
        winner=sqlite3_column_text(stmt,2);
        fprintf(fp,"Last winner: %s\n", winner);
        fprintf(fp,"----------------------------------------\n");
        sqlite3_close(db);
	    }
      rc = sqlite3_finalize(stmt);
      sqlite3_close(db);
  fclose(fp);
}

void delete_file()
{
  if (remove("championships.txt") == 0)
      printf("Deleted successfully\n");
   else
      printf("Already deleted\n");

  if (remove("history.txt") == 0)
      printf("Deleted successfully\n");
   else
      printf("Already deleted\n");
}

size_t read_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  FILE *readhere = (FILE *)userdata;
  curl_off_t nread;
  size_t retcode = fread(ptr, size, nmemb, readhere);
  nread = (curl_off_t)retcode;
  fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T " bytes from file\n", nread);
  return retcode;
}

int verify_name(char command[])
{
  const char *user;
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db,"select name from championships",-1,&stmt,0);
  while(sqlite3_step(stmt)!=SQLITE_DONE)
	    {
		    user=sqlite3_column_text(stmt,0);
        printf("Verifying every championship name\n");
		    if(strstr(command,user)!=0) 
		      {
            return 1;
          }
        //sqlite3_close(db);
	    }
      rc = sqlite3_finalize(stmt);
      sqlite3_close(db);
  return 0;
}

void create_email(char command[])
{
  FILE *fp = fopen("email.txt", "w+");
  fprintf(fp,"\n\n");
  sqlite3_stmt *stmt;
  const char *name;
  const char *type;
  const char *nb;
  const char *structure;
  const char *date;
  const char *ora;
  const char *desc;  
  sqlite3_prepare_v2(db,"select name, type,nb_players, structure, games, ora,desc from championships",-1,&stmt,0);
  while(sqlite3_step(stmt)!=SQLITE_DONE)
	    {
		    name=sqlite3_column_text(stmt,0);
		    if(strstr(command,name)!=0) 
		      {
            type=sqlite3_column_text(stmt,1);
            if(strstr(type,weak)==0){
            fprintf(fp, "Congratulations, %s!\n", username);
            fprintf(fp, "You've been accepted to this championship. See details below:\n");
            fprintf(fp,"----------------------------------------\n");
            fprintf(fp, "Name: %s\n", name);
            fprintf(fp, "Type: %s\n", type);
            nb=sqlite3_column_text(stmt,2);
            fprintf(fp, "Number of players: %s\n", nb);
            structure=sqlite3_column_text(stmt,3);
            fprintf(fp, "Structure: %s\n", structure);
            date=sqlite3_column_text(stmt,4);
            fprintf(fp, "Date: %s\n", date);
            ora=sqlite3_column_text(stmt,5);
            fprintf(fp, "Hour: %s\n", ora);
            desc=sqlite3_column_text(stmt,6);
            fprintf(fp, "Short description: %s\n", desc);
            fprintf(fp,"----------------------------------------\n");
            fprintf(fp, "Good luck!\n");
            }
            else {
              fprintf(fp, "Hello %s,\n You haven't been accepted to %s.\n",username, name);
              fprintf(fp, "Try another championship, following your strong points: %s.\n", strong);
              fprintf(fp, "Good luck!\n");
              fprintf(fp,"----------------------------------------\n");
            }
          }
        // sqlite3_close(db);
	    }
    rc = sqlite3_finalize(stmt);
      sqlite3_close(db);
  fclose(fp);
}
void send_email ()
{
    printf("Request for participation\n");
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "smtp.mail.yahoo.com");  
    curl_easy_setopt(curl, CURLOPT_USERNAME, "dariae9@yahoo.com");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "uldnkvnyfaomrukj");
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "dariae9@yahoo.com");
    struct curl_slist *recipients = NULL;
    recipients = curl_slist_append(recipients, "dariae9@yahoo.com");
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    
    FILE *fp = fopen("email.txt", "rb");
    
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, (void *)fp);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    
    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
          fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    else printf("Email sent\n");
    
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    fclose(fp);
    if (remove("email.txt") == 0)
      printf("Deleted successfully\n");
     else
      printf("Already deleted\n");
  }
}

void case_answer(int idThread,char command[]){

  if(strstr(command,"show championships")!= NULL &&  v[idThread].login == 1){
    printf("Showing championships\n");
    show_championships();
    strcpy(command,"The list of championships\n");
  }
  else if(strstr(command,"show championships")!= NULL &&v[idThread].login == 0){
    printf("Not logged in\n");
    strcpy(command,"Please login first");
  }
  else if(strstr(command,"participate")!= NULL && v[idThread].admin == 1){
    printf("Admin mode\n");
    strcpy(command,"You're on admin mode\n");
  }
  else if(strstr(command,"participate")!= NULL && v[idThread].normal == 1){
    participate_initiated = 1;
    strcpy(command,"Write the name of the championship");
  }
  else if(strstr(command,"reschedule championship")!= NULL && v[idThread].normal == 0){
    printf("Not logged in\n");
    strcpy(command,"Please login first");
  }
  else if(strstr(command,"reschedule championship")!= NULL && v[idThread].normal == 1){
    printf("Reschedule options\n");
    strcpy(command,"Rescheduling");
  }
  else if(strstr(command,"create championship")!= NULL && v[idThread].admin == 0){
    printf("Not logged in\n");
    strcpy(command,"Please login as admin");
  }
  else if(strstr(command,"create championship")!= NULL && v[idThread].admin == 1){
    printf("Created\n");
    create_initiated = 1;
    strcpy(command,"Write the details");
  }
  else if(strstr(command,"edit championship")!= NULL && v[idThread].admin == 1){
    printf("Edited\n");
    strcpy(command,"Write the name of the championship");
    editing_initiated = 1;
  }
  else if(strstr(command,"edit championship")!= NULL && v[idThread].admin == 0){
    printf("Not logged in\n");
    strcpy(command,"Please login as admin");
  }
  else if(strstr(command,"history")!= NULL && v[idThread].admin == 1){
    printf("History\n");
    show_history();
    strcpy(command,"History");
  }
  else if(strstr(command,"history")!= NULL && v[idThread].admin == 0){
    printf("Not logged in\n");
    strcpy(command,"Please login as admin");
  }
  else if(strstr(command,"quit")!=NULL){
      printf("Clientul %d pleaca\n",idThread);
      strcpy(command,"Goodbye"); 
      delete_file();
    }
	else if( strstr(command,"login")!=NULL && v[idThread].login == 0){
      printf("Login initiated\n");
      strcpy(command,"Write your username");
      login_initiated = 1;
    }
  else if(strstr(command,"login")!=NULL && v[idThread].login == 1){
      printf("Already logged\n");
      strcpy(command,"Already logged in\n");
    }
  else if(strstr(command,"logout")!=NULL && v[idThread].login == 1){
      printf("Logout succesfully\n");
      v[idThread].login = 0;
      v[idThread].admin = 0;
      v[idThread].normal = 0;
      delete_file();
      strcpy(command,"Logged out\n");
    }
  else if(strstr(command,"logout")!=NULL && v[idThread].login == 0){
      printf("Logout failed\n");
      strcpy(command,"Not logged in\n");
    }
  else if (login_initiated == 1){ // cauta username-ul
     if(search_username(idThread, command)==1){
        pass_initiated = 1;
     }
     login_initiated = 0;
  }
  else if(pass_initiated == 1 ){
    pass_initiated = 0;
    search_password(idThread, command);
  }
  else if(editing_initiated == 1){
      char nume[200];
      strcpy(nume,command);
      if(verify_name(nume)==1){
        strcpy(command,"Editing begins");
      }
      else {
        strcpy(command,"This championship doesn't exist");
      }
      editing_initiated = 0;
  }
  else if(participate_initiated == 1){
    pthread_mutex_lock(&lock);
    if(verify_name(command)){
      create_email(command);
      send_email();
      strcpy(command,"Request sent. Check your email to see details");
    }
    else{
      strcpy(command,"Doesn't exist");
    }
    pthread_mutex_unlock(&lock);
    participate_initiated = 0 ;
  }
  else{ // nu recunoaste nicio comanda
      printf("Unknown command\n");
      strcpy(command,"Unknown command, try again\n");
    }
}
void raspunde(void *arg)
{
  int nr, i=0;
  char command[200];
	struct thData tdL; 
	tdL= *((struct thData*)arg);
  
  //citeste informatia de la client
  while(1){
    memset(command, 0, sizeof(command));

	  if (read (tdL.cl, &command,sizeof(command)) <= 0) 
		  	{
			    printf("[Thread %d]\n",tdL.idThread);
			    printf ("Eroare la read() de la client.\n");
			
			  }
	    
    
    //raspundem pe cazuri
    case_answer(tdL.idThread,command);

	  if (write (tdL.cl, &command, sizeof(command)) <= 0)
		  {
		  printf("[Thread %d] ",tdL.idThread);
		  printf("[Thread] Eroare la write() catre client.\n");
		  }

    if(strstr(command,"Goodbye")!=NULL){
      break; // iesim din while pentru acest client
    }
  }
}

static void *treat(void * arg) /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
{		
		struct thData tdL; 
		tdL= *((struct thData*)arg);	
		printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
		fflush (stdout);		 
		pthread_detach(pthread_self());		
		raspunde((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	
}


int main ()
{
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in cli;	    //structura folosita de client
  int nr;		                  //mesajul primit de trimis la client 
  int sd;		                  //descriptorul de socket 
  int pid;
  pthread_t th[100];          //Identificatorii thread-urilor care se vor crea
  int i=0;
  
  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      printf ("[server] Eroare la socket().\n");
      exit(1);
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&cli, sizeof (cli));
  
  int port = 2009;
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); 
  /* utilizam un port utilizator */
    server.sin_port = htons (port);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      printf ("[server] Eroare la bind().\n");
      exit(1);
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 2) == -1)
    {
      printf ("[server] Eroare la listen().\n");
      exit(1);
    }

  //opening the database containing all the information

  rc = sqlite3_open("test.db", &db);
  if(rc){
    printf("Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }
  else{
    printf("Opened database\n");
  }

  /* servim in mod concurent clientii...folosind thread-uri */
  while (1)
    {
      int client;
      thData * td; //parametru functia executata de thread     
      int length = sizeof (client); //lg clientului

      printf ("[server] Asteptam la portul %d...\n",port);
      fflush (stdout); //curatam stdout-ul

      // client= malloc(sizeof(int));
      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      if ( (client = accept (sd, (struct sockaddr *) &cli, &length)) < 0)
	{
	  printf ("[server] Eroare la accept().\n");
	  continue;
	}
	// int idThread; //id-ul threadului
	// int cl; //descriptorul intors de accept
  //in struct incrementez numarul de thread-uri si ii asignez clientul curent
	td=(struct thData*)malloc(sizeof(struct thData));	
	td->idThread=i++;
	td->cl=client;

	pthread_create(&th[i], NULL, &treat, td);	      
				
	}
  sqlite3_close(db);
  return 0; 
}