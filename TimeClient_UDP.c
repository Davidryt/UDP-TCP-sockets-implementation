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

#define BUFSIZE 1024

int sockfd;

void sigint_handler(int signo)
{
	/* things to do after a Ctrl-C, before finishing the program*/
	puts("SIGINT received...");
	close(sockfd);
	exit(0);
}

int main(int argc, char *argv[])
{
	printf("WELCOME\n");
	char* host, *mode;
	int port = 37;
    //char buf[BUFSIZE+1];
    
	short debug = 0;

        /* handler SIGINT signal*/
	//signal(SIGINT, sigint_handler);

	/* check command line arguments */
	if(argc > 8 || argc < 5){
		printf("Número de parámetros incorrecto\n");
		exit(0);
	}

    int opt;

    while ((opt = getopt(argc, argv, "s:p:m:d")) != -1) {
        switch (opt) {
        case 's': host = optarg; break;
        case 'p': port = atoi(optarg); break;
        case 'm': mode = optarg; break;
		case 'd': debug = 1; break;
        default:
            fprintf(stderr, "Usage: error al introducir las opciones", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

	if(strcmp(mode, "cu") == 0){
		clienteUDP(debug, host, port);
	}else if(strcmp(mode, "ct") == 0){
		clienteTCP(debug, host, port);
	}else if(strcmp(mode, "s") == 0){
		serverTCP(debug, host, port);
	}else
		printf("Invalid mode");

}

void clienteUDP(short debug,  char * host, int port){

	struct hostent *server;
	struct sockaddr_in serveraddr;
	
	printf("Cliente UDP\n");
	/* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
		printf("Enviamos datagrama vacío al servidor\n");
	
	uint32_t msg;
	int n = sendto(sockfd, &msg, (size_t)4, 0, (struct sockaddr *) &serveraddr, (socklen_t) sizeof(struct sockaddr_in));
	
	if (n < 1) {
            perror("ERROR sending message (sendto)");
            exit(0);
    }

	int outchars, inchars;
	outchars=4; //Son los 32 bits que esperamos recibir	

	if(debug == 1)
		printf("Recibimos datagrama del servidor\n");

	socklen_t len = (socklen_t) sizeof(struct sockaddr_in);
	for(inchars=0; inchars < outchars;inchars+=n){
		n = recvfrom(sockfd, &msg, (size_t)4, 0, (struct sockaddr *) &serveraddr, &len);
		if (n < 0) {
            perror("ERROR receiving message (receivefrom)");
            exit(0);
   		}
	}
	
	if(debug == 1){
		printf("Convertimos los segundos recibidos en la fecha actual\n");
		printf("msg: %d\n", msg);
		printf("msg addr: %d\n", &msg);
		printf("htons msg: %d\n", ntohl(msg));
	}

	msg = ntohl(msg);
	time_conversion(&msg);	

	close(sockfd);
}


void clienteTCP(short debug,  char * host, int port){

	struct hostent *server;
	struct sockaddr_in serveraddr;

	signal(SIGINT, sigint_handler);
	
	printf("Cliente TCP\n");
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

	/* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
            perror("ERROR connecting");
            exit(0);
    }
	
	if(debug == 1)
		printf("Enviamos datagrama vacío al servidor\n");
	
	uint32_t msg;
	int n = send(sockfd, &msg, (size_t)4, 0);
	
	if (n < 1) {
            perror("ERROR sending message (sendto)");
            exit(0);
    }

	int outchars, inchars;
	outchars=4; //Son los 32 bits que esperamos recibir	

	if(debug == 1)
		printf("Recibimos datagrama del servidor\n");
	
	while(1){
		for(inchars=0; inchars < outchars;inchars+=n){
			n = recv(sockfd, &msg, (size_t)4, 0);
			if (n < 0) {
		        perror("ERROR receiving message (receivefrom)");
		        exit(0);
	   		}
		}
		
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

void serverTCP(short debug,  char * host, int port){

		printf("Server init\n");

	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
        int clientlen; // byte size of client's address
        struct sockaddr_in serveraddr; // server's addr
        struct sockaddr_in clientaddr; // client addr
        struct hostent *hostp; // client host info
        char buf[BUFSIZE]; // message buffer
        char *hostaddrp; // dotted decimal host addr string
        int optval; // flag value for setsockopt
        int cc;
		int ss;
		time_t time_send;

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


	        /* read: read input string from the client */
                while (cc = recv(new_fd, buf, sizeof(buf),0)) {
						printf("Received pack with cc %d\n", cc);
                        if (cc < 0){
                            fprintf(stderr, "ERROR echo read: %s\n",
				        		strerror(errno));
                            exit(1);
						}
						
						while(1){
							time_send = time(NULL);
							printf("sending time %d\n", time_send);
							ss=send(new_fd,(char *)time_send, cc, 0);
							if (ss < 0){
                                fprintf(stderr, "ERROR echo write: %s\n",
				        		strerror(errno));
                                exit(1);
								}
							printf("sent, sleeping\n");
							sleep(1);
						}
                        //write(0,buf,cc); /* stdin is the 0 file descriptor */

			/* echo the input string back to the client */
                       
                }
                //close(new_fd);
		//perror("CIERRO EL SOCKET DEL CLIENTE, VUELVO A ESPERAR...");
	}

}














void time_conversion(uint32_t * seconds){

	time_t time = *seconds - 2208988800;
	printf("The time is: %s\n", ctime(&time));

}






