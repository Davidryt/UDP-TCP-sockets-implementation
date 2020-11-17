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

#include "serverTCP.h"
#include "atdate.h"
int sockfd;
int new_fd;  // listen on sock_fd, new connection on new_fd

void sigint_handler_s(int signo)
{
	/* things to do after a Ctrl-C, before finishing the program*/
	puts("\nSIGINT received, closing program");
	close(sockfd);
	close(new_fd);
	exit(0);
}

void serverTCP(short debug,  char * host, int port){

		printf("Server init\n");


        int clientlen; // byte size of client's address
        struct sockaddr_in serveraddr; // server's addr
        struct sockaddr_in clientaddr; // client addr
        struct hostent *hostp; // client host info
        char buf[BUFSIZE]; // message buffer
        char *hostaddrp; // dotted decimal host addr string
        int optval; // flag value for setsockopt
        int cc;
		int ss;
		uint32_t time_send;

		signal(SIGINT, sigint_handler_s);

        /* socket: create the parent socket */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
                perror("ERROR opening socket");
			exit(0);
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

        /* listen: make this socket ready to accept connection requests */
        if (listen(sockfd, 1) < 0) {
                perror("ERROR on listen");
		exit(0);
		}

        /* wait for a connection request */
		while(1) {  // main accept() loop
			printf("MAIN LOOP\n");
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
                //printf("server got connection from %s (%s)\n",
		//       hostp->h_name, hostaddrp);


			if (!fork()) { //we only one to execute this part by the child
				close(sockfd)
				while(new_fd != -1){
			 		time_send = htonl((uint32_t) time(NULL)+2208988800);
					printf("sending time %d\n", time_send);
					ss=send(new_fd,&time_send, sizeof(time_send), 0);
					if (ss < 0){
                		fprintf(stderr, "ERROR echo write: %s\n",
					    strerror(errno));
                	    exit(1);
					}

					if (ss = 0){
                		fprintf(stderr, "ERROR send 0 %s\n",
						strerror(errno));
                	    exit(1);
					}

					printf("sent, sleeping\n");
					sleep(1);
				}
				close(new_fd);
				exit(0);
			}
			

			/* echo the input string back to the client */

          }
                //close(new_fd);
		//perror("CIERRO EL SOCKET DEL CLIENTE, VUELVO A ESPERAR...");
	}
