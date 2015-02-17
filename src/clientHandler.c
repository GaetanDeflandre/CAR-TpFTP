#include "clientHandler.h"
#include <command.h>
#include <servFTP.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

ssize_t read_client_request(int sockfd, char **request);
//ssize_t write_reply(int sockfd, char *reply);

void handle_client(struct sockaddr_in client_addr, int socket)
{
	struct s_client client;
	struct s_cmd * cmd = NULL;
	char buf[BUF_SIZE+1]; // +1 pour '\0'
	char * request = NULL;
	int req_size;
	
	// Création de la structure client.
	memset(client.cli_current_path, 0, PATHNAME_MAXLEN);
	client.cli_sock = socket;
	client.cli_addr = client_addr;
	client.cli_data_port = client.cli_addr.sin_port;
	client.cli_data_transfer_t = NORMAL_DT;
	
	/* Salutations au client */
	snprintf(buf, BUF_SIZE, "220 Service ready\r\n");
    if(write(client.cli_sock, buf, strlen(buf)) == -1)
    {
		perror("Erreur write: ");
		exit(EXIT_FAILURE);
    }
    
    while(1)
    {
		/* Lecture requete */
		req_size = read_client_request(client.cli_sock, &request);
		printf("Size of request: %d\nRequest: %s\n", req_size, request);
		
		/* Traitement de la requete */
		cmd = init_cmd(request, &client);
		if (cmd != NULL)
			exec_cmd(cmd);
		else
		{
			fprintf(stderr, "Erreur cmd\n");
			close_connection(&client);
		}
			
		if (cmd->cmd_t == CMD_QUIT)
			return;
		
		/* TODO Reponse au client */
		snprintf(buf, BUF_SIZE, "230\r\n");
		
		if(write(client.cli_sock, buf, strlen(buf)) == -1)
		{
			perror("Erreur write: ");
			exit(EXIT_FAILURE);
		}
	}
}

ssize_t read_client_request(int sockfd, char **request)
{
	char buf[BUF_SIZE+1]; // +1 pour '\0'
	ssize_t nb_bytes_read, remaining_space;
	int nb_written_char, request_size;
	
	/* Allocation d'une nouvelle zone memoire */
	free(*request);
	remaining_space = request_size = 10;
	nb_written_char = 0;
	*request = (char *) malloc(remaining_space+1 * sizeof(char));
	if (*request == NULL)
	{
		perror("Erreur malloc: ");
		exit(EXIT_FAILURE);
	}
	
	do
	{
		nb_bytes_read = read(sockfd, buf, BUF_SIZE);
		if(nb_bytes_read == -1)
		{
			perror("Erreur read: ");
			exit(EXIT_FAILURE);
		}
		
		if (nb_bytes_read > remaining_space)
		{
			*request = (char *) realloc(*request, nb_written_char + nb_bytes_read + request_size);
			if (*request == NULL)
			{
				perror("Erreur realloc: ");
				exit(EXIT_FAILURE);
			}
		}
		
		memcpy((*request) + nb_written_char, buf, nb_bytes_read);
		nb_written_char += nb_bytes_read;
		remaining_space -= nb_bytes_read;
		
	} while(buf[nb_bytes_read - 2] != '\r' || buf[nb_bytes_read - 1] !='\n');
	/* Test de fin de requete (\r\n à la fin). */
    
    (*request)[nb_written_char] = '\0';
    
    return strlen(*request);
}

void close_connection(struct s_client * client)
{
	close(client->cli_sock);
	
	return;
}
