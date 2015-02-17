#include "clientHandler.h"
#include <command.h>
#include <servFTP.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void handle_client(struct sockaddr_in client_addr, int socket)
{
	struct s_client client;
	struct s_cmd * cmd;
	char buf[BUF_SIZE+1]; // +1 pour '\0'
	ssize_t nb_bytes_read;
	
	// Création de la structure client.
	memset(client.cli_current_path, 0, MAX_PATH_LEN);
	client.cli_sock = socket;
	client.cli_addr = client_addr;
	client.cli_data_port = client.cli_addr.sin_port;
	client.cli_data_transfer_t = NORMAL_DT;
	
	/* Salutations au client */
	snprintf(buf, sizeof(buf), "220 Service ready\r\n");
    if(write(client.cli_sock, buf, strlen(buf)) == -1){
		perror("Erreur write: ");
		exit(EXIT_FAILURE);
    }

    if((nb_bytes_read = read(client.cli_sock, buf, BUF_SIZE)) == -1){
		perror("Erreur read: ");
		exit(EXIT_FAILURE);
    }
    
    cmd = init_cmd(buf, &client);
    if (cmd != NULL)
		exec_cmd(cmd);
	else
		printf("Erreur cmd\n");
    
    snprintf(buf, sizeof(buf), "230\r\n");
    
    if(write(client.cli_sock, buf, strlen(buf)) == -1){
	perror("Erreur write: ");
	exit(EXIT_FAILURE);
    }
    
    close(client.cli_sock);
    
    return;
}
