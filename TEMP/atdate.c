/*
 * atdate.c
 * Aplicaciones Telematicas - Grado en Ingenieria Telematica
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "Cliente_TCP.h"
#include "Servidor_TCP.h"
#include "Cliente_UDP.h"

#define CLIENTE_UDP 1
#define CLIENTE_TCP 2
#define SERVIDOR_TCP 3

#define PUERTO_POR_DEFECTO_CLIENTE_UDP 37
#define PUERTO_POR_DEFECTO_SERVER_TCP 6518

int main(int argc, char *argv[])
{
	char *host;
	int modo = CLIENTE_UDP;
	int port = PUERTO_POR_DEFECTO_CLIENTE_UDP;
	int debug = 0;
	int options;
	char *modo_str;
	extern char* optarg;


	while ((options = getopt(argc, argv, "h:p:m:d")) != -1){
		switch(options){
			case 'h':
				host = optarg;
				break;

			case 'p':
				port = atoi(optarg);
				break;

			case 'm':
				modo_str = optarg;
				if(strcmp(modo_str, "cu") == 0){
					modo = CLIENTE_UDP;
				}else if(strcmp(modo_str, "ct") == 0){
					modo = CLIENTE_TCP;
				}else if(strcmp(modo_str, "s") == 0){
					modo = SERVIDOR_TCP;
				}else {
						printf("Modo incorrecto\n");
					  }
				break;

			case 'd':
			debug = 1;
			break;

      case '?':
        printf("Uso: atdate [-h serverhost] [-p port] [-m mode]\n");
        printf("[-h serverhost]: nombre del servidor TIME al que se conectará el programa para obtener la fecha y hora actual. 0bligatorio sólo en modo consulta\n");
        printf("[-p port]: para indicar que el servidor TIME al que nos conectamos escucha en un puerto diferente al 37. El puerto por defecto es el 37.\n");
        printf("[-m mode]: para indicar el modo de ejecución del programa:\n");
        printf("[-m] cu: el programa arranca en modo consulta funcionando como cliente UDP.\n");
        printf("[-m] ct: el programa arranca en modo consulta funcionando como cliente TCP.\n");
        printf("[-m] s: el programa arranca en modo servidor.\n");
        printf("Si no se especifica el programa arranca en modo consulta UDP.\n");
        printf("[-d]: modo depuración.\n");
				exit(2);

		}
	}
	if(modo == CLIENTE_UDP){
		if(host == NULL){
			printf("No se ha indicado el host\n");
			exit(2);
		}else{
			cliente_udp(host, PUERTO_POR_DEFECTO_CLIENTE_UDP, debug);
		}
	}

	if(modo == CLIENTE_TCP){
		if(host == NULL){
			printf("No se ha indicado el host\n");
			exit(2);
		}else{
			cliente_tcp(host, port, debug);
		}

	}

	if(modo == SERVIDOR_TCP){
		if(port != PUERTO_POR_DEFECTO_SERVER_TCP && port >= 1024){
			servidor_tcp(port, debug);

		}else{
			servidor_tcp(PUERTO_POR_DEFECTO_SERVER_TCP, debug);
		}
	}

	return 0;
}
