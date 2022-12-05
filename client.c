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

int port = 2009;

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
  printf ("[client]Introduceti comanda: ");
  fflush (stdout);
  read (0, command, sizeof(command));
  
  printf("[client] Am citit %s\n",command);
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
  printf ("[client]Mesajul primit este: %s\n", command);
  if(strstr(command,"Goodbye")!=NULL){
    close (sd);
    break;
  }

  memset(command, 0, sizeof(command));
  }
  
}