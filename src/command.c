#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "servFTP.h"
#include "command.h"
#include "communication.h"
#include "database.h"
#include "dtp.h"

#define CODE_USER "USER"
#define CODE_PASS "PASS"
#define CODE_QUIT "QUIT"
#define CODE_SYST "SYST"
#define CODE_PORT "PORT"
#define CODE_LIST "LIST"

/* CREATION DE COMMANDES */
struct s_cmd * new_user(char * args);
struct s_cmd * new_pass(char * args);
struct s_cmd * new_quit(char * args);
struct s_cmd * new_syst(char * args);
struct s_cmd * new_port(char * args);
struct s_cmd * new_list(char * args);

/* HANDLERS DES COMMANDES */
void process_user(struct s_cmd * cmd);
void process_pass(struct s_cmd * cmd);
void process_quit(struct s_cmd * cmd);
void process_syst(struct s_cmd * cmd);
void process_port(struct s_cmd * cmd);
void process_list(struct s_cmd * cmd);

void not_implemented_command(struct s_client * client);

struct s_cmd * init_cmd(char * client_request, struct s_client * client)
{
	struct s_cmd * cmd;
	char *request_code, *request_args;
	char * crlf;
	char request_line[MAX_REQUEST_LENGTH];
	
	/* Si la requete est trop longue. */
	if (strlen(client_request) > MAX_REQUEST_LENGTH -1)
	{
		fprintf(stderr, "Erreur init_cmd: La requete est trop longue.\n");
		return NULL;
	}
	
	strncpy(request_line, client_request, MAX_REQUEST_LENGTH);
	
	/* Elimination du CRLF en fin de requete. */
	crlf = strstr(request_line, "\r\n");
	if (crlf != NULL)
		*crlf = '\0';
	else
	{
		fprintf(stderr, "Erreur init_cmd: La requete ne termine pas par <CRLF>.\n");
		return NULL;
	}
	
	request_code = strtok(request_line, " \f\n\r\t\v");
	request_args = strtok(NULL, ""); // Le reste de la chaine.
	
	if (strncasecmp(request_code, CODE_USER, strlen(request_code)) == 0)
	{
		cmd = new_user(request_args);
	}
	else if (strncasecmp(request_code, CODE_PASS, strlen(request_code)) == 0)
	{
		cmd = new_pass(request_args);
	}
	else if (strncasecmp(request_code, CODE_QUIT, strlen(request_code)) == 0)
	{
		cmd = new_quit(request_args);
	}
	else if (strncasecmp(request_code, CODE_SYST, strlen(request_code)) == 0)
	{
		cmd = new_syst(request_args);
	}
	else if (strncasecmp(request_code, CODE_PORT, strlen(request_code)) == 0)
	{
		cmd = new_port(request_args);
	}
	else if (strncasecmp(request_code, CODE_LIST, strlen(request_code)) == 0)
	{
		cmd = new_list(request_args);
	}
	else
	{
	    not_implemented_command(client);
	    return NULL;
	}
	
	if (cmd != NULL)
	{
		cmd->cmd_client = client;
	}
		
	return cmd;
}

void not_implemented_command(struct s_client * client){
    
    char buf[BUF_SIZE];

    /* Command not implemented. */
    snprintf(buf, BUF_SIZE, "502 Command not implemented.\r\n");
	
    if(write(client->cli_sock, buf, strlen(buf)) == -1){
	perror("Erreur write: ");
	return;
    }
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
		request_args = malloc((strlen(args) + 1) * sizeof(char));
		strcpy(request_args, args);
	}
		
	cmd->cmd_t = CMD_USER;
	cmd->cmd_h = process_user;
	cmd->cmd_args_field = request_args;
	
	return cmd;
}

struct s_cmd * new_pass(char * args)
{
	struct s_cmd * cmd;
	char * request_args = NULL;
	
	printf("new_pass\n");
	
	cmd = malloc(sizeof(struct s_cmd));
	if (cmd == NULL)
	{
		fprintf(stderr, "Erreur new_user: Erreur d'allocation de memoire.\n");
		return NULL;
	}
	
	if (args != NULL)
	{
		request_args = malloc((strlen(args) + 1) * sizeof(char));
		strcpy(request_args, args);
	}
		
	cmd->cmd_t = CMD_PASS;
	cmd->cmd_h = process_pass;
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

struct s_cmd * new_syst(char * args)
{
    struct s_cmd * cmd;
    char * request_args = NULL;

    cmd = malloc(sizeof(struct s_cmd));
    if (cmd == NULL){
	fprintf(stderr, "Erreur new_syst: Erreur d'allocation de memoire.\n");
	return NULL;
    }

    if (args != NULL){
	request_args = malloc((strlen(args) + 1) * sizeof(char));
	strcpy(request_args, args);
    }

    cmd->cmd_t = CMD_SYST;
    cmd->cmd_h = process_syst;
    cmd->cmd_args_field = request_args;
	
    return cmd;
}

struct s_cmd * new_port(char * args)
{
    struct s_cmd * cmd;
    char * request_args = NULL;

    cmd = malloc(sizeof(struct s_cmd));
    if (cmd == NULL){
	fprintf(stderr, "Erreur new_port: Erreur d'allocation de memoire.\n");
	return NULL;
    }

    if (args != NULL){
	request_args = malloc((strlen(args) + 1) * sizeof(char));
	strcpy(request_args, args);
    }

    cmd->cmd_t = CMD_PORT;
    cmd->cmd_h = process_port;
    cmd->cmd_args_field = request_args;
	
    return cmd;
}

struct s_cmd * new_list(char * args)
{
    struct s_cmd * cmd;
    char * request_args = NULL;

    cmd = malloc(sizeof(struct s_cmd));
    if (cmd == NULL){
	fprintf(stderr, "Erreur new_list: Erreur d'allocation de memoire.\n");
	return NULL;
    }

    if (args != NULL){
	request_args = malloc((strlen(args) + 1) * sizeof(char));
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
	char * password;
	char * path;
	char buf[BUF_SIZE+1];
	
	/* Si la requete contient plusieurs arguments. */
	if (cmd->cmd_args_field == NULL || strtok(cmd->cmd_args_field, " \f\n\r\t\v") != cmd->cmd_args_field)
	{
		snprintf(buf, BUF_SIZE, "501 Syntax error in parameters or arguments.\r\n");
		write_socket(cmd->cmd_client->cli_sock, buf);
		
		return;
	}
	
	printf("Recherche de l'utilisateur %s\n", cmd->cmd_args_field);
	
	if (get_user_info(cmd->cmd_args_field, &password, &path))
	{
		cmd->cmd_client->cli_username = (char *) malloc((strlen(cmd->cmd_args_field) + 1) * sizeof(char));
		strcpy(cmd->cmd_client->cli_username, cmd->cmd_args_field);
		snprintf(buf, BUF_SIZE, "331 User name okay, need password.\r\n");
		write_socket(cmd->cmd_client->cli_sock, buf);
	}
	else
	{
		snprintf(buf, BUF_SIZE, "530 Not logged in. Unknown username.\r\n");
		write_socket(cmd->cmd_client->cli_sock, buf);
	}
	
	return;
}

void process_pass(struct s_cmd * cmd)
{
	char * password;
	char * path;
	char buf[BUF_SIZE+1];
	
	/* Si la requete contient plusieurs arguments. */
	if (cmd->cmd_args_field == NULL || strtok(cmd->cmd_args_field, " \f\n\r\t\v") != cmd->cmd_args_field)
	{
		snprintf(buf, BUF_SIZE, "501 Syntax error in parameters or arguments.\r\n");
		write_socket(cmd->cmd_client->cli_sock, buf);
		
		return;
	}
	
	printf("Verification mdp %s\n", cmd->cmd_args_field);
	
	if (get_user_info(cmd->cmd_client->cli_username, &password, &path))
	{
		if (strcmp(password, cmd->cmd_args_field) == 0)
		{
			cmd->cmd_client->cli_logged_in = 1;
			strncpy(cmd->cmd_client->cli_current_path, path, PATHNAME_MAXLEN);
			snprintf(buf, BUF_SIZE, "230 User logged in, proceed.\r\n");
			write_socket(cmd->cmd_client->cli_sock, buf);
		}
		else
		{
			free(cmd->cmd_client->cli_username);
			memset(cmd->cmd_client->cli_current_path, 0, PATHNAME_MAXLEN);
			cmd->cmd_client->cli_username = NULL;
			snprintf(buf, BUF_SIZE, "530 Not logged in. Bad password.\r\n");
			write_socket(cmd->cmd_client->cli_sock, buf);
		}
	}
	else
	{
		snprintf(buf, BUF_SIZE, "530 Not logged in. Unknown username.\r\n");
		write_socket(cmd->cmd_client->cli_sock, buf);
	}
	
	return;
}

void process_quit(struct s_cmd * cmd)
{
	char buf[BUF_SIZE+1];
	
	printf("Fin connexion\n");
    snprintf(buf, BUF_SIZE, "221 Service closing control connection.\r\n");
	write_socket(cmd->cmd_client->cli_sock, buf);
	close_connection(cmd->cmd_client);
	
	return;
}

void process_syst(struct s_cmd * cmd)
{
    char buf[MESSAGE_SIZE];

    /* NAME system type. */
    snprintf(buf, BUF_SIZE, "215 linux\r\n");
    
    if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	perror("Erreur write: ");
	return;
    }
    return;
}

void process_port(struct s_cmd * cmd)
{
    char buf[MESSAGE_SIZE];
    /*char addr[NB_AGRS_PORT][3];
      unsigned len, i;*/

    if(cmd->cmd_args_field == NULL){
	/* Syntax error in parameters or arguments. */
	snprintf(buf, BUF_SIZE, "501 Syntax error in parameters or arguments.\r\n");

	if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	    perror("Erreur write: ");
	    return;
	}
    }

    /*len = strlen(cmd->cmd_args_field)
	for(i=0; ){

    }

    printf("args: %s\n", cmd->cmd_args_field);*/


    /*snprintf(buf, BUF_SIZE, "215 linux\r\n");
    
    if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	perror("Erreur write: ");
	return;
	}*/
    return;
}

void process_list(struct s_cmd * cmd)
{
    DIR *pDir;
    struct dirent *pDirent;
    char buf[MESSAGE_SIZE];
    char path[PATHNAME_MAXLEN];
    int status = 0;
    struct s_client * client;
    struct s_data_connection * c_data_connect;
    
    client = cmd->cmd_client;

    if(client == NULL){
	fprintf(stderr, "Erreur: client faut null.\n");
	status = -1;
    }

    /* ARGUMENT */
    if(strcpy(path, client->cli_current_path) == NULL){
	perror("Erreur strcpy: ");
	status = -1;
    }
    if(cmd->cmd_args_field != NULL){
	if(strcat(path, "/") == NULL){
	    perror("Erreur strcat: ");
	    status = -1;
	}
	if(strcat(path, cmd->cmd_args_field) == NULL){
	    perror("Erreur strcat: ");
	    status = -1;
	}
    }

    /* OPEN DIR */
    if(status>=0){
	pDir = opendir(path);
	if (pDir == NULL) {
	    perror("Erreur opendir: ");
	    status = -2;
	}
    }

    /* GET FOLDER CHILDREN  */
    if(status>=0){
	if((pDirent = readdir(pDir)) != NULL){

	    if(strncpy(buf, pDirent->d_name, MESSAGE_SIZE-1) == NULL){
		perror("Erreur strncpy: ");
		status = -1;
	    }

	    if(strcat(buf, "  ")== NULL){
		perror("Erreur strcat: ");
		status = -1;
	    }
	
	    while ((pDirent = readdir(pDir)) != NULL) {
		if(strcat(buf, pDirent->d_name)== NULL){
		    perror("Erreur strcat: ");
		    status = -1;
		    break;
		}
		if(strcat(buf, "  ")== NULL){
		    perror("Erreur strcat: ");
		    status = -1;
		    break;
		}
	    }

	    strcat(buf, "\r\n");

	} else {
	    buf[0] = '\0';
	}
    }

    if(status>=0){

	/* SEND LIST*/
	/* File status okay; about to open data connection. */
	snprintf(buf, BUF_SIZE, "125 File status okay; about to open data connection.\r\n");

	if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	    perror("Erreur write: ");
	    status = -1;
	}

	c_data_connect = client->cli_data_connection;
	if(c_data_connect == NULL){
	    fprintf(stderr, "Erreur: champ cli_data_connection null.\n");
	    status = -1;
	}
	if(c_data_connect->dc_transfer_t == DT_ACTIVE){

	    if(open_data_connection(c_data_connect) == -1){
		fprintf(stderr, "Erreur: Ouverture connection data.\n");
		status = -1;
	    }

	    write_data(buf, c_data_connect);

	    if(close_data_connection(c_data_connect) == -1){
		fprintf(stderr, "Erreur: Fermeture connection data.\n");
		status = -1;
	    }

	    /* Closing data connection. */
	    snprintf(buf, BUF_SIZE, "226 Closing data connection.\r\n");

	    if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
		perror("Erreur write: ");
		return;
	    }
	    return;

	} else {
	    /* Command not implemented for that parameter. */
	    snprintf(buf, BUF_SIZE, "504 Command not implemented for that parameter.\r\n");
	
	    if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
		perror("Erreur write: ");
		status = -1;
	    }

	}
    }
    
    if(status == 0){
	/* Closing data connection. */
	snprintf(buf, BUF_SIZE, "125 Closing data connection.\r\n");
	
	if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	    perror("Erreur write: ");
	    return;
	}

    } else if(status == -1){
	/* Connection closed; transfer aborted. */
	snprintf(buf, BUF_SIZE, "426 Connection closed; transfer aborted.\r\n");

	if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	    perror("Erreur write: ");
	    return;
	}
    } else if(status == -2){
	/* Syntax error in parameters or arguments. */
	snprintf(buf, BUF_SIZE, "501 Syntax error in parameters or arguments.\r\n");

	if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	    perror("Erreur write: ");
	    return;
	}
    }

    return;
}

void destroy_cmd(struct s_cmd * cmd)
{
	if (cmd != NULL)
	{
		free(cmd->cmd_args_field);
		free(cmd);
	}
}
