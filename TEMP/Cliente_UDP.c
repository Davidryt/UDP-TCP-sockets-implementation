/*
 * Cliente_UDP.c
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


#define DATE_TIME 2208988800

int sockfd;

int cliente_udp( char* host, int port, int debug)
{
	struct hostent *server;
	struct sockaddr_in serveraddr;
  int num_bytes;
  long tiempo = 0;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(0);
  }

  if(debug){
    printf("Socket creado\n");
  }

  server = gethostbyname(host);
  if (server == NULL) {
     fprintf(stderr,"ERROR, no such host: %s\n", host);
     exit(0);
  }

  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
  serveraddr.sin_port = htons(port);

  if(debug){
    printf("Configurado Server Address\n");
  }

  // variables del time
  struct tm* info;
  char buf[80];
  size_t valor = 0;
  unsigned int addrserver_size = sizeof(serveraddr);

  if(debug){
    printf("Inicializar valires del time\n");
  }

  while(valor == 0){
    num_bytes = sendto(sockfd, &tiempo, sizeof(long), 0,
     (struct sockaddr *) &serveraddr, addrserver_size);

    if(num_bytes < 0){
      perror("ERROR reading from socket");
    }

    if(debug){
      printf("Envia mensaje\n");
    }

    num_bytes = recvfrom(sockfd, &tiempo, sizeof(long), 0,
     (struct sockaddr *) &serveraddr, &addrserver_size);

    if (num_bytes < 0) {
      perror("ERROR reading from socket");
      exit(0);
		}

		if(debug){
			printf("Leer respuesta del servidor\n");
		}

		tiempo = ntohl(tiempo);
		tiempo = tiempo - DATE_TIME;
		info = localtime((time_t *) &tiempo);
		valor = strftime(buf, 80, "%a %b %d %X %Z %Y", info);
		printf("%s\n", buf);
	}

	close(sockfd);
	return(0);

}
