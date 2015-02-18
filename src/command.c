#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>

#include "command.h"
#include "database.h"

#define CODE_USER "USER"
#define CODE_QUIT "QUIT"
#define CODE_LIST "LIST"

/* CREATION DE COMMANDES */
struct s_cmd * new_user(char * args);
struct s_cmd * new_quit(char * args);
struct s_cmd * new_list(char * args);

/* HANDLERS DES COMMANDES */
void process_user(struct s_cmd * cmd);
void process_quit(struct s_cmd * cmd);
void process_list(struct s_cmd * cmd);

struct s_cmd * init_cmd(char * client_request, struct s_client * client)
{
	struct s_cmd * cmd;
	char *request_code, *request_args;
	char * cr_char;
	char request_line[MAX_LINE_LENGTH];
	
	strncpy(request_line, client_request, MAX_LINE_LENGTH);
	
	request_code = strtok(request_line, " ");
	request_args = strtok(NULL, "");
	
	/* Elimination de CRLF, si il est trouve a la fin du code de 
	requete, pour la reconnaissance de code */
	cr_char = strrchr(request_code, '\r');
	if (cr_char != NULL)
	{
		if (*(cr_char+1) == '\n')
			*cr_char = '\0';
	}
	
	if (strncasecmp(request_code, CODE_USER, strlen(request_code)) == 0)
	{
		cmd = new_user(request_args);
	}
	else if (strncasecmp(request_code, CODE_QUIT, strlen(request_code)) == 0)
	{
		cmd = new_quit(request_args);
	}
	else if (strncasecmp(request_code, CODE_LIST, strlen(request_code)) == 0)
	{
		cmd = new_list(request_args);
	}
	else
	{
		return NULL;
	}
	
	if (cmd != NULL)
	{
		cmd->cmd_client = client;
	}
		
	return cmd;
}

void exec_cmd(struct s_cmd * cmd)
{
	cmd->cmd_h(cmd);
}


/* CREATION DE COMMANDES */

struct s_cmd * new_user(char * args)
{
	struct s_cmd * cmd;
	char * request_args = NULL;
	
	printf("new_user\n");
	
	cmd = malloc(sizeof(struct s_cmd));
	if (cmd == NULL)
	{
		fprintf(stderr, "Erreur new_user: Erreur d'allocation de memoire.\n");
		return NULL;
	}
	
	if (args != NULL)
	{
		request_args = malloc(strlen(args) * sizeof(char) + 1);
		strcpy(request_args, args);
	}
		
	cmd->cmd_t = CMD_USER;
	cmd->cmd_h = process_user;
	cmd->cmd_args_field = request_args;
	
	return cmd;
}

struct s_cmd * new_quit(char * args)
{
	struct s_cmd * cmd;
	
	cmd = malloc(sizeof(struct s_cmd));
	
	if (cmd == NULL)
	{
		fprintf(stderr, "Erreur new_quit: Erreur d'allocation de memoire.\n");
		return NULL;
	}
	
	cmd->cmd_t = CMD_QUIT;
        cmd->cmd_h = process_quit;
	cmd->cmd_args_field = NULL;
	
	return cmd;
}

struct s_cmd * new_list(char * args)
{
    struct s_cmd * cmd;
    char * request_args = NULL;

    cmd = malloc(sizeof(struct s_cmd));
    if (cmd == NULL){
	fprintf(stderr, "Erreur new_user: Erreur d'allocation de memoire.\n");
	return NULL;
    }

    if (args != NULL){
	request_args = malloc(strlen(args) * sizeof(char) + 1);
	strcpy(request_args, args);
    }

    cmd->cmd_t = CMD_LIST;
    cmd->cmd_h = process_list;
    cmd->cmd_args_field = request_args;
	
    return cmd;
}


/* HANDLERS DES COMMANDES */

void process_user(struct s_cmd * cmd)
{
	printf("Ok %s\n", cmd->cmd_args_field);
	strncpy(cmd->cmd_client->cli_current_path, "somewhere", PATHNAME_MAXLEN);
	printf("Chemin : %s\n", cmd->cmd_client->cli_current_path);
	
	return;
}

void process_quit(struct s_cmd * cmd)
{
	printf("Fin connexion\n");
       
	close_connection(cmd->cmd_client);
	
	return;
}

void process_list(struct s_cmd * cmd)
{
    DIR *pDir;
    
    pDir = opendir("files");
    if (pDir == NULL) {
        perror("Erreur opendir: ");
	return;
    }

    printf("process_list\n");
    return;
}
