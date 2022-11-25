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

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

void raspunde(void *arg)
{
    int nr, i=0;
	struct thData tdL; 
	tdL= *((struct thData*)arg);
    //citeste informatia de la client
	if (read (tdL.cl, &nr,sizeof(int)) <= 0) 
			{
			  printf("[Thread %d]\n",tdL.idThread);
			  printf ("Eroare la read() de la client.\n");
			
			}
	
	printf ("[Thread %d] Mesajul a fost receptionat...%d\n",tdL.idThread, nr);	      
	/*pregatim mesajul de raspuns */
	nr++;      
	printf("[Thread %d] Trimitem mesajul inapoi...%d\n",tdL.idThread, nr);
	/* returnam mesajul clientului */
	 if (write (tdL.cl, &nr, sizeof(int)) <= 0)
		{
		 printf("[Thread %d] ",tdL.idThread);
		 printf("[Thread] Eroare la write() catre client.\n");
		}
	else
		printf ("[Thread %d] Mesajul a fost trasmis cu succes.\n",tdL.idThread);	

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