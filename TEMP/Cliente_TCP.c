/*
 * Cliente_TCP.c
 * Aplicaciones Telematicas - Grado en Ingeneria Telematica
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

#define DATE_TIME 2208988800

int sockfd;

void sigint_handler(int signo)
{
	/* things to do after a Ctrl-C, before finishing the program*/
	puts("SIGINT received...");
	exit(0);
}


int cliente_tcp( char* host, int port, int debug)
{
	struct hostent *server;
	struct sockaddr_in serveraddr;
  int num_bytes_recibidos;
  long tiempo = 0;


    /* handler SIGINT signal*/
	signal(SIGINT, sigint_handler);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(0);
    }

		if(debug){
			printf("Socket creado\n");
		}

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(host);
    if (server == NULL) {
       fprintf(stderr,"ERROR, no such host: %s\n", host);
       exit(0);
    }

		if(debug){
			printf("Consigue entrada DNS del servidor\n");
		}

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(port);

		if(debug){
			printf("Configurado Server Address\n");
		}

    /* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        perror("ERROR connecting");
        exit(0);
    }

		if(debug){
			printf("Creada conexion con el servidor\n");
		}

    // variables del time
    struct tm* info;
    char buf[80];

    /* get message line from the user */
    while (1) {

    /* send the message line to the server */
    /* x = send(sockfd, buf, outchars,0);
    if (x < 0) {
    	perror("ERROR writing to socket");
      exit(0);
    }
   */
	 /* read answer from the server */

    	num_bytes_recibidos = recv(sockfd, &tiempo, sizeof(long), 0);

			if (num_bytes_recibidos < 0) {
      	perror("ERROR reading from socket");
        exit(0);
      }

      if (num_bytes_recibidos == 0) {
				perror("Server closed connection");
				exit(0);
      }

			if(debug){
				printf("Respuesta del servidor\n");
			}

      tiempo = ntohl(tiempo);
      tiempo = tiempo - DATE_TIME;
      info = localtime((time_t *) &tiempo);
      strftime(buf, 80, "%a %b %d %X %Z %Y", info);

      /* print the server's reply */
      printf("%s\n", buf);

		}

    close(sockfd);
    return 0;
}
