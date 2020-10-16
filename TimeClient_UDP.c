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
/*
void sigint_handler(int signo)
{
	// things to do after a Ctrl-C, before finishing the program
	puts("SIGINT received...");
	exit(0);
}
*/

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
	printf("0.5: %d\n", argc);
	if(argc > 8 || argc < 5){
		printf("Número de parámetros incorrecto\n");
		exit(0);
	}
	printf("1\n");

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
	
/*
	switch (argc) {
	case 8:
		host = argv[2];
		port = atoi(argv[4]);//si no lo recibimos lo fijamos en 37
		mode = argv[6];
		debug_str = argv[7];
		
		break;
	default:
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(1);
    }
	
	if(memcmp(debug, "-d", sizeof(debug)) == 0)
		debug =1;
*/
	printf("preif: %s\n", mode);
	if(strcmp(mode, "cu") == 0){
		printf("entramos en el cliente\n");
		clienteUDP(debug, host, port);
	}
/*
	switch(mode){
	case "cu":
		clienteUDP(debug, host, port);
		break;
	case "ct":
		//TCP()
		break;
	case "s":
		//server()
		break;		
	default:
		fprintf(stderr, "usage: %s incorrect \n ", mode);
		exit(1);

	}
*/
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
	
	if(debug == 1)
		printf("Convertimos los segundos recibidos en la fecha actual\n");

	printf("msg: %d\n", msg);
	printf("msg addr: %d\n", &msg);
	printf("htons msg: %d\n", ntohl(msg));
	msg = ntohl(msg);
	time_conversion(&msg);	

	close(sockfd);
}





void time_conversion(uint32_t * seconds){

	printf("1");
	time_t time = *seconds - 2208988800;
	printf("2\n");
	printf("The time is: %s\n", ctime(&time));

}






