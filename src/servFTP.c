#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <signal.h>

#include <errno.h>

#include "servFTP.h"
#include <clientHandler.h>

/* Private */

/* Utilisateur de la commande */
void usage(const char* name);
/* Initialise le handler pour attendre les fils */
int init_sigaction(struct sigaction * action);
/* Fonction pour attrendre les fils */
void wait_child(int sig);

void usage(const char* name){
    printf("Serveur FTP.\n\n");
    printf("Utilisation:\n");
    printf("\t$ %s\n", name);
}

int init_sigaction(struct sigaction * action){
    action->sa_handler = wait_child;
    if(sigemptyset( &(action->sa_mask) ) == -1){
	perror("Erreur sigemptyest: ");
	return -1;
    }
    action->sa_flags = SA_RESTART;

    return 0;
}

void wait_child(int sig){
    int status;
    waitpid(-1, &status, WNOHANG);
}

int launch_server(){

    int listenfd, clientfd;
    int sock_opt_reuse_addr = 1;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t sock_len;
    struct sigaction action;
    pid_t cpid;
    
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
    
    if(init_sigaction(&action) == -1){
	fprintf(stderr, "Erreur: lors de l'initialisation du handler wait_child.\n");
	return -1;
    }

    if(sigaction(SIGCHLD, &action, NULL) == -1){
	perror("Erreur signaction: ");
	return -1;
    }
    
    while(1){

		clientfd = accept(listenfd, (struct sockaddr *) &cli_addr, &sock_len);
		if(clientfd == -1){
			perror("Erreur accept: ");
			return -1;
		}

		printf("Nouveau client\n");
		   
		cpid = fork();
		switch (cpid)
		{
			case -1: /* Erreur */
			perror("Erreur fork: ");
			return -1;

			case 0: /* Fils */
			handle_client(cli_addr, clientfd);
			close(clientfd);
		
			return 0;
			break;
		
			default: /* Père */
				close(clientfd);
		}
    }
    
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

    exit(EXIT_SUCCESS);
}
