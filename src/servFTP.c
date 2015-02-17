#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include "servFTP.h"
#include <clientHandler.h>

void usage(char* name){
    printf("Serveur FTP.\n\n");
    printf("Utilisation:\n");
    printf("\t$ %s", name);
}

int main(int argc, char* argv[]){
    int listenfd, clientfd, status; 
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t sock_len;
    
    char buf[BUF_SIZE];  

    if(argc != 1){
	usage(argv[0]);
	exit(EXIT_SUCCESS);
    }
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (listenfd == -1){
	perror("Erreur socket: ");
	exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(struct sockaddr));
    memset(buf, 0, BUF_SIZE);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(2121);
	
    if ((bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) != 0){
	perror("Erreur bind: ");
	exit(EXIT_FAILURE);
    }
	
    listen(listenfd, 10);
	
	sock_len = sizeof(cli_addr);
	

	clientfd = accept(listenfd, (struct sockaddr *) &cli_addr, &sock_len);
	
	switch (fork())
	{
		case 0:
			handle_client(cli_addr, clientfd);
			close(clientfd);
			
			exit(EXIT_SUCCESS);
			break;
	}
	
	wait(&status);
    

    //read(clientfd, buf, BUF_SIZE);
    //~ printf("%s\n", buf);
    close(clientfd);
	
    close(listenfd);

    exit(EXIT_SUCCESS);
}
