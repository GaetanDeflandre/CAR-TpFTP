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

void wait_child(int sig){
  int status;

  waitpid(-1,&status,WNOHANG);
}

int launch_server(){

    int listenfd, clientfd, status;
    int sock_opt_reuse_addr = 1;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t sock_len;
    
    char buf[BUF_SIZE+1]; // +1 pour '\0'

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &sock_opt_reuse_addr, sizeof(int));
    
    if (listenfd == -1){
	perror("Erreur socket: ");
	return -1;
    }

    memset(&serv_addr, 0, sizeof(struct sockaddr));
    memset(buf, 0, BUF_SIZE);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_PORT);
	
    if ((bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) != 0){
	perror("Erreur bind: ");
	return -1;
    }
	
    listen(listenfd, 10);
	
    sock_len = sizeof(cli_addr);

    signal(SIGCHLD, wait_child);

    clientfd = accept(listenfd, (struct sockaddr *) &cli_addr, &sock_len);
	
    switch (fork())
	{
	case 0: /* Fils */
	    handle_client(cli_addr, clientfd);
	    close(clientfd);
	
	    exit(EXIT_SUCCESS);
	    break;
	
	case -1: /* Erreur */
	    perror("Erreur fork: ");
	    return -1;
	}
    
    /* Père */
    wait(&status);
    
    close(clientfd);	
    close(listenfd);

    return 0;
}

int main(int argc, char* argv[]){

    if(argc != 1){
	usage(argv[0]);
	exit(EXIT_SUCCESS);
    }

    if(launch_server() == -1){
	exit(EXIT_FAILURE);
    }

    /*int listenfd, clientfd, status;
    int sock_opt_reuse_addr = 1;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t sock_len;
    
    char buf[BUF_SIZE+1]; // +1 pour '\0'

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &sock_opt_reuse_addr, sizeof(int));
    
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
	
    close(listenfd);*/

    exit(EXIT_SUCCESS);
}
