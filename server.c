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


//variabile globale
int login = 0;   //pt logare
int normal = 0; //clienti normali
int admin = 0; //admini

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

void case_answer(int idThread,char command[200]){

  if(strstr(command,"show championships")!= NULL && login == 1){
    printf("Showing championships\n");
    strcpy(command,"The list of championships\n");
  }
  else if(strstr(command,"show championships")!= NULL && login== 0){
    printf("Not logged in\n");
    strcpy(command,"Please login first");
  }
  else if(strstr(command,"participate")!= NULL && normal == 0){
    printf("Not logged in\n");
    strcpy(command,"Please login first");
  }
  else if(strstr(command,"participate")!= NULL && normal == 1){
    printf("Request for participation\n");
    strcpy(command,"Request sent. Check your email to see details");
  }
  else if(strstr(command,"reschedule")!= NULL && normal == 0){
    printf("Not logged in\n");
    strcpy(command,"Please login first");
  }
  else if(strstr(command,"reschedule")!= NULL && normal == 1){
    printf("Reschedule options\n");
    strcpy(command,"Rescheduling");
  }
  else if(strstr(command,"create")!= NULL && admin == 0){
    printf("Not logged in\n");
    strcpy(command,"Please login as admin");
  }
  else if(strstr(command,"create")!= NULL && admin == 1){
    printf("Created\n");
    strcpy(command,"Championship created");
  }
  else if(strstr(command,"edit")!= NULL && admin == 1){
    printf("Edited\n");
    strcpy(command,"Edited details");
  }
  else if(strstr(command,"edit")!= NULL && admin == 0){
    printf("Not logged in\n");
    strcpy(command,"Please login as admin");
  }
  else if(strstr(command,"history")!= NULL && admin == 1){
    printf("History\n");
    strcpy(command,"History");
  }
  else if(strstr(command,"history")!= NULL && admin == 0){
    printf("Not logged in\n");
    strcpy(command,"Please login as admin");
  }
  else if(strstr(command,"quit")!=NULL){
      printf("Clientul %d pleaca\n",idThread);
      strcpy(command,"Goodbye"); //trimite clientului
    }
	else if( strstr(command,"login")!=NULL && login == 0){
      printf("Login initiated\n");
      login = 1;
      admin = 1;
      normal = 1;
      strcpy(command,"Welcome back\n");
    }
  else if(strstr(command,"login")!=NULL && login == 1){
      printf("Already logged\n");
      strcpy(command,"Already logged in\n");
    }
  else if(strstr(command,"logout")!=NULL && login == 1){
      printf("Logout succesfully\n");
      login = 0;
      admin = 0;
      normal = 0;
      strcpy(command,"Logged out\n");
    }
  else if(strstr(command,"logout")!=NULL && login == 0){
      printf("Logout failed\n");
      login = 0;
      strcpy(command,"Not logged in\n");
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
	
  	printf ("[Thread %d] Mesajul a fost receptionat...%s\n",tdL.idThread, command);	      
    
    //raspundem pe cazuri
    case_answer(tdL.idThread,command);

	  printf("[Thread %d] Trimitem mesajul inapoi...%s\n",tdL.idThread, command);
	  /* returnam mesajul clientului */
	  if (write (tdL.cl, &command, sizeof(command)) <= 0)
		  {
		  printf("[Thread %d] ",tdL.idThread);
		  printf("[Thread] Eroare la write() catre client.\n");
		  }
	  else {
       printf ("[Thread %d] Mesajul a fost trasmis cu succes.\n",tdL.idThread);
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
  int nr;		//mesajul primit de trimis la client 
  int sd;		//descriptorul de socket 
  int pid;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
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
    return 0; 
}