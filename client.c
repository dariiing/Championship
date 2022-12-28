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
  // else if(strstr(command,"Write the details")!=NULL){
  //   char sql[400] = "INSERT INTO CHAMPIONSHIPS (NAME,TYPE,NB_PLAYERS,STRUCTURE,HISTORY,GAMES) VALUES ('";
  //   sqlite3_stmt *stmt;
  //   memset(command, 0, sizeof(command));
  //   read (0, command, sizeof(command));
  //   command[strlen(command)-1]='\0';
  //   strcat(sql,command);//nume
  //   strcat(sql,"'");
  //   strcat(sql,",");
  //   memset(command, 0, sizeof(command));
  //   read (0, command, sizeof(command));
  //   command[strlen(command)-1]='\0';
  //   strcat(sql,"'");
  //   strcat(sql,command); //tip
  //   strcat(sql,"'");
  //   strcat(sql,",");
  //   memset(command, 0, sizeof(command));
  //   read (0, command, sizeof(command));
  //   command[strlen(command)-1]='\0';
  //   strcat(sql,command);//nb
  //   strcat(sql,",");
  //   memset(command, 0, sizeof(command));
  //   read (0, command, sizeof(command));
  //   command[strlen(command)-1]='\0';
  //   strcat(sql,"'");
  //   strcat(sql,command);//struct
  //   strcat(sql,"'");
  //   strcat(sql,",");
  //   memset(command, 0, sizeof(command));
  //   read (0, command, sizeof(command));
  //   command[strlen(command)-1]='\0';
  //   strcat(sql,"'");
  //   strcat(sql,command);//history
  //   strcat(sql,"'");
  //   strcat(sql,",");
  //   memset(command, 0, sizeof(command));
  //   read (0, command, sizeof(command));
  //   command[strlen(command)-1]='\0';
  //   strcat(sql,"'");
  //   strcat(sql,command);//games
  //   strcat(sql,"');");
  //   printf("%s\n",sql);
  //   int rc =sqlite3_exec(db,sql,0,0,&zErrMsg);
  //   if( rc != SQLITE_DONE ){
  //     printf("SQL error: %s\n", zErrMsg);
  //  } else {
  //     printf("Records created successfully\n");
  //  }
  // }
  else if(strstr(command,"Goodbye")!=NULL){
    close (sd);
    break;
  }
  memset(command, 0, sizeof(command));
  }
  
}