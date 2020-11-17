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

#include "clientUDP.h"
#include "clientTCP.h"
#include "serverTCP.h"
#include "atdate.h"


int main(int argc, char *argv[])
{
	printf("WELCOME\n");
	char* host, *mode;
	mode = "cu";
	int port = 37;
    //char buf[BUFSIZE+1];
    
	short debug = 0;

        /* handler SIGINT signal*/
	//signal(SIGINT, sigint_handler);

	/* check command line arguments */
	/*
	if(argc > 8 || argc < 5){
		printf("Número de parámetros incorrecto\n");
		exit(0);
	}
	*/

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



void time_conversion(uint32_t * seconds){

	time_t time = *seconds - 2208988800;
	printf("The time is: %s\n", ctime(&time));

}

