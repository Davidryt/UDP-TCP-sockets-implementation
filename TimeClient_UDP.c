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

#define BUFSIZE 1024

int sockfd;

void sigint_handler(int signo)
{
	/* things to do after a Ctrl-C, before finishing the program*/
	puts("SIGINT received...");
	exit(0);
}


int main(int argc, char *argv[])
{
	struct hostent *server;
	struct sockaddr_in serveraddr;
	char* host;
	int port;
    char buf[BUFSIZE+1];
    int n;
    int outchars, inchars;

        /* handler SIGINT signal*/
	signal(SIGINT, sigint_handler);
