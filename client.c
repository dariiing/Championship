#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sqlite3.h>

int port = 2009;
sqlite3 *db;
char *zErrMsg = 0;

void insert_championship(){

  int rc = sqlite3_open("test.db", &db);
  if(rc){
    printf("Can't open database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  char sql[4000];
    printf("Name: ");
    fflush (stdout);
    char name[200];
    memset(name, 0, sizeof(name));
    read (0, name, sizeof(name));
    name[strlen(name)-1]= '\0';
    
    printf("Type: ");
    fflush (stdout);
    char type[200];
    memset(type, 0, sizeof(type));
    read (0, type, sizeof(type));
    type[strlen(type)-1]= '\0';
    
    printf("Structure: ");
    fflush (stdout);
    char structure[200];
    memset(structure, 0, sizeof(structure));
    read (0, structure, sizeof(structure));
    structure[strlen(structure)-1]= '\0';
    
    printf("Last played: ");
    fflush (stdout);
    char history[200];
    memset(history, 0, sizeof(history));
    read (0, history, sizeof(history));
    history[strlen(history)-1]= '\0';

    printf("Last Winner: ");
    fflush (stdout);
    char winner[200];
    memset(winner, 0, sizeof(winner));
    read (0, winner, sizeof(winner));
    winner[strlen(winner)-1]= '\0';

    printf("Date: ");
    fflush (stdout);
    char date[200];
    memset(date, 0, sizeof(date));
    read (0, date, sizeof(date));
    date[strlen(date)-1]= '\0';

    printf("Hour: ");
    fflush (stdout);
    char ora[200];
    memset(ora, 0, sizeof(ora));
    read (0, ora, sizeof(ora));
    date[strlen(date)-1]= '\0';

    printf("Number of players: ");
    fflush (stdout);
    int nr;
    read (0, (void*)&nr, 2);

    sprintf(sql, "INSERT INTO CHAMPIONSHIPS (NAME, TYPE, STRUCTURE, HISTORY, WINNER, GAMES,ORA, NB_PLAYERS) VALUES ('%s','%s','%s','%s','%s','%s','%s', %d);", name, type, structure, history, winner, date, ora, nr);
    printf("%s\n",sql);
    rc =sqlite3_exec(db,sql,0,0,&zErrMsg);
    if( rc != SQLITE_OK ){
      printf("SQL error: %s\n", zErrMsg);
   } else {
      printf("Records created successfully\n");
   }
   sqlite3_close(db);
}

void read_file(char command[])
{
  FILE *fp;
  if(strstr(command,"History")!=NULL){
    fp = fopen("history.txt","r");
  }
  else{
    fp = fopen("championships.txt","r");
  }
  char buffer[256];
  while (fgets(buffer, 256, fp)){
        printf("%s", buffer);
  }
  printf("\n\n\n");
  fclose(fp);
}

int editing_client = 0;
int main ()
{
  int sd;			                // descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  char command[200];

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      printf("Eroare la socket().\n");
      exit(1);
    }

  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* portul de conectare */
  server.sin_port = htons (port);
  
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      printf ("[client]Eroare la connect().\n");
      exit(1);
    }

  while(1){
  
  printf ("CLIENT:\t");
  fflush (stdout);
  read (0, command, sizeof(command));

  char nume_edit[200];
  if(editing_client == 1){
      strcpy(nume_edit, command);
      nume_edit[strlen(nume_edit)-1]='\0';
  }
  //printf("[client] Am citit %s\n",command);
  if (write (sd,&command,sizeof(command)) <= 0)
    {
      printf("[client]Eroare la write() spre server.\n");
      exit(1);
    }

  if (read (sd, &command,sizeof(command)) < 0)
    {
      printf ("[client]Eroare la read() de la server.\n");
      exit(1);
    }
    
  //verificam ce a trimis serverul
  printf ("SERVER:\t%s\n", command);
  if(strstr(command,"The list of championships")!=NULL || strstr(command,"History")!=NULL){
      read_file(command);
  }
  else if(strstr(command,"Write the details")!=NULL){
    insert_championship();
  }
  else if(strstr(command,"Write the name of the championship")!=NULL){
    editing_client = 1;
  }
  else if(strstr(command,"Editing begins")!=NULL){
    update_championship(editing_client, command,nume_edit);
  }
  else if(strstr(command,"Goodbye")!=NULL){
    close (sd);
    break;
  }
  memset(command, 0, sizeof(command));
  }
  sqlite3_close(db);
}