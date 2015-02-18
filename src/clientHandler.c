#include "clientHandler.h"
#include "command.h"
#include "communication.h"
#include "servFTP.h"
#include "dtp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int needing_login_cmd(struct s_cmd * cmd);
ssize_t read_client_request(int sockfd, char **request);
//ssize_t write_reply(int sockfd, char *reply);

void handle_client(struct sockaddr_in client_addr, int socket)
{
	struct s_client client;
	struct s_cmd * cmd = NULL;
	char buf[BUF_SIZE+1]; // +1 pour '\0'
	char * request = NULL;
	int req_size;
	int waitingForPassword = 0;
	
	// Création de la structure client.
	client.cli_username = NULL;
	client.cli_logged_in = 0;
	memset(client.cli_current_path, 0, PATHNAME_MAXLEN);
	client.cli_sock = socket;
	client.cli_data_connection = new_data_connection(client_addr);
		
	/* Salutations au client */
	snprintf(buf, BUF_SIZE, "220 Service ready\r\n");
	write_socket(client.cli_sock, buf);
    
    /* Boucle de traitement de requetes. Une requete par tour. */
    while(1)
    {
		/* Lecture requete */
		req_size = read_client_request(client.cli_sock, &request);
		printf("Size of request: %d\nRequest: %s\n", req_size, request);
		
		/* Traitement de la requete */
		destroy_cmd(cmd);
		cmd = init_cmd(request, &client);
		if (cmd != NULL)
		{
			/* Si la requete necessite un login */
			if (needing_login_cmd(cmd))
			{
				if (!client.cli_logged_in)
				{
					snprintf(buf, BUF_SIZE, "530 Not logged in.\r\n");
					write_socket(client.cli_sock, buf);
					
					continue;
				}
			}
			else if (cmd->cmd_t == CMD_PASS)
			{
				if (!waitingForPassword)
				{
					snprintf(buf, BUF_SIZE, "503 Bad sequence of commands.\r\n");
					write_socket(client.cli_sock, buf);
					
					continue;
				}
			} else if (cmd->cmd_t == CMD_USER) // (Re)commencement du login.
			{
				memset(client.cli_current_path, 0, PATHNAME_MAXLEN);
				free(client.cli_username);
				client.cli_username = NULL;
				client.cli_logged_in = 0;
			}
			
			/* Execution de la requete */
			exec_cmd(cmd);
			
			if (waitingForPassword)
			{
				if (!client.cli_logged_in)
				{
					memset(client.cli_current_path, 0, PATHNAME_MAXLEN);
					free(client.cli_username);
					client.cli_username = NULL;
				}
				waitingForPassword = 0;
			}
			
			/* Commande USER avec succes. */
			if (cmd->cmd_t == CMD_USER && client.cli_username != NULL)
				waitingForPassword = 1;
		}
		else
		{
			/* Requete incorrecte. */
		    fprintf(stderr, "Erreur cmd: request=%s\n", request);
		    snprintf(buf, BUF_SIZE, "500 Syntax error, command unrecognized.\r\n");
			write_socket(client.cli_sock, buf);
			
			if (waitingForPassword)
			{
				if (!client.cli_logged_in)
				{
					memset(client.cli_current_path, 0, PATHNAME_MAXLEN);
					free(client.cli_username);
					client.cli_username = NULL;
				}
				waitingForPassword = 0;
			}
		}
		
		/* Connexion terminée. */
		if (cmd != NULL && cmd->cmd_t == CMD_QUIT)
			return;
	}
}

int needing_login_cmd(struct s_cmd * cmd)
{
	if (cmd->cmd_t == CMD_CWD || cmd->cmd_t == CMD_CDUP
		|| cmd->cmd_t == CMD_LIST || cmd->cmd_t == CMD_MKD
		|| cmd->cmd_t == CMD_RMD || cmd->cmd_t == CMD_RETR
		|| cmd->cmd_t == CMD_STOR)
	{
		return 1;
	}
	
	return 0;
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
	if (is_data_connection_opened(client->cli_data_connection))
		close_data_connection(client->cli_data_connection);
		
	free(client->cli_username);
	client->cli_username = NULL;
	close(client->cli_sock);
	
	return;
}
