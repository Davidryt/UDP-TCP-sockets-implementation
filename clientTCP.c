#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include "clientTCP.h"
#include "atdate.h"
int sockfd;

void sigint_handler_ct(int signo)
{
	/* things to do after a Ctrl-C, before finishing the program*/
	puts("\nSIGINT received, closing program");
	close(sockfd);
	exit(0);
}

void clienteTCP(short debug,  char * host, int port){

	struct hostent *server;
	struct sockaddr_in serveraddr;

	signal(SIGINT, sigint_handler_ct);
	
	printf("Cliente TCP 10\n");
	/* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
            perror("ERROR opening socket");
            exit(0);
    }

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(host);
    if (server == NULL) {
            fprintf(stderr,"ERROR, no such host: %s\n", host);
            exit(0);
    }
	
	/* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(port);

	if(debug == 1)
		printf("Intentamos conexion con server\n");

	/* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
            perror("ERROR connecting");
            exit(0);
    }

	if(debug == 1)
		printf("Conexión exitosa\n");

	int outchars, inchars;
	outchars=4; //Son los 32 bits que esperamos recibir	
	
	int n;
	uint32_t msg;

	while(1){
		//for(inchars=0; inchars < outchars;inchars+=n){
			if(debug == 1)
				printf("Esperando recepción\n");

			n = recv(sockfd, &msg, (size_t)4, 0);
			if (n < 0) {
		        perror("ERROR receiving message (recev)");
		        exit(0);
	   		}

      		if (n == 0) {
				perror("Server closed connection");
				exit(0);
      		}

			if(debug==1){
				printf("Respuesta del servidor\n");
			}
		//}
		
			if(debug == 1){
				printf("Convertimos los segundos recibidos en la fecha actual\n");
				printf("msg: %d\n", msg);
				printf("msg addr: %d\n", &msg);
				printf("htons msg: %d\n", ntohl(msg));
			}

			msg = ntohl(msg);
			time_conversion(&msg);	

	}
}
