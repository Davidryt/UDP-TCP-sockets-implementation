/*
 * Servidor_TCP.c
 * Aplicaciones Telematicas - Grado en Ingenieria Telematica
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
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 10	 // how many pending connections queue will hold
#define DATE_TIME 2208988800

void sigchld_handler(int s);
int TCPechod(int fd);
int new_fd;

int servidor_tcp(int port, int debug)
{
  int sockfd;  // listen on sock_fd, new connection on new_fd
  int clientlen; // byte size of client's address
  struct sockaddr_in serveraddr; // server's addr
  struct sockaddr_in clientaddr; // client addr
  struct hostent *hostp; // client host info
  char *hostaddrp; // dotted decimal host addr string
  int optval; // flag value for setsockopt

	/* socket: create the parent socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
  	perror("ERROR opening socket");
		exit(0);
	}

	if(debug)
	{
		printf("Creado el socket\n");
	}

  /* setsockopt: lets s rerun the server immediately after we kill it.
	 * Eliminates "ERROR on binding: Address already in * use"
	 * error. */
  optval = 1;
  setsockopt(new_fd, SOL_SOCKET, SO_REUSEADDR,
		(const void *)&optval , sizeof(int));

  /* build the server's Internet address */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)port);

  /* bind: associate the parent socket with a port */
  if (bind(sockfd, (struct sockaddr *) &serveraddr,
	         sizeof(serveraddr)) < 0) {
  				 perror("ERROR on binding");
					 exit(0);
	}

	if(debug)
	{
		printf("Construido el Server Address\n");
	}

  /* listen: make this socket ready to accept connection requests */
  if (listen(sockfd, BACKLOG) < 0) {
  	perror("ERROR on listen");
		exit(0);
	}

	printf("TIME server running in port %i\n", port);

	if(debug){
		printf("server: listening on port:%i\n", port);
	}

	if(debug){
		printf("Prepara al socket para conectarlo\n");
	}

	/* handler SIGCHLD signal */
	signal(SIGCHLD, sigchld_handler);

	if(debug)
	{
		printf("Señal SIGCHLD\n");
	}
  /* wait for a connection request */
	while(1) {  // main accept() loop
  	clientlen = sizeof(clientaddr);
		new_fd = accept(sockfd, (struct sockaddr *)&clientaddr,
		                &clientlen);
		if (new_fd == -1) {
			perror("ERROR on accept");
			exit(0);
		}

		/* gethostbyaddr: determine who sent the message */
  	hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
		sizeof(clientaddr.sin_addr.s_addr), AF_INET);

  	if (hostp == NULL) {
  		perror("ERROR on gethostbyaddr");
    	exit(0);
		}

  	hostaddrp = inet_ntoa(clientaddr.sin_addr);

  	if (hostaddrp == NULL) {
  		perror("ERROR on inet_ntoa\n");
			exit(0);
		}

		if(debug)
		{
			printf("Determina quien mandó el mensaje\n");
		}

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			TCPechod(new_fd);
			close(new_fd);
			exit(0);
		}

	close(new_fd);  // parent doesn't need this

	}
	return 0;
}

/*------------------------------------------------------------------------
 * sigchld_handler - reaper of zombi child processes
 *------------------------------------------------------------------------
 */
void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

/*------------------------------------------------------------------------
 * TCPechod - echo data until end of file
 *------------------------------------------------------------------------
 */
int TCPechod(int fd)
{
	uint32_t tiempo;
    time_t time_process;

    while (new_fd != -1) {

        time_process = time(NULL);
        tiempo = time_process + DATE_TIME;
        tiempo = htonl(tiempo);

		/* echo the input string back to the client */
        if (send(fd, &tiempo,sizeof(tiempo), 0) < 0){
            fprintf(stderr, "ERROR echo write: %s\n",
            strerror(errno));
            exit(1);
		}

		sleep(1);
    }

	return 0;
}
