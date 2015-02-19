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
#define CODE_RETR "RETR"
#define CODE_STOR "STOR"

/* CREATION DE COMMANDES */
struct s_cmd * new_user(char * args);
struct s_cmd * new_pass(char * args);
struct s_cmd * new_quit(char * args);
struct s_cmd * new_syst(char * args);
struct s_cmd * new_port(char * args);
struct s_cmd * new_list(char * args);
struct s_cmd * new_retr(char * args);
struct s_cmd * new_stor(char * args);

/* HANDLERS DES COMMANDES */
void process_user(struct s_cmd * cmd);
void process_pass(struct s_cmd * cmd);
void process_quit(struct s_cmd * cmd);
void process_syst(struct s_cmd * cmd);
void process_port(struct s_cmd * cmd);
void process_list(struct s_cmd * cmd);
void process_retr(struct s_cmd * cmd);
void process_stor(struct s_cmd * cmd);

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
	else if (strncasecmp(request_code, CODE_RETR, strlen(request_code)) == 0)
	{
		cmd = new_retr(request_args);
	}
	else if (strncasecmp(request_code, CODE_STOR, strlen(request_code)) == 0)
	{
		cmd = new_stor(request_args);
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

struct s_cmd * new_retr(char * args)
{
	struct s_cmd * cmd;
	char * request_args = NULL;
	
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
		
	cmd->cmd_t = CMD_RETR;
	cmd->cmd_h = process_retr;
	cmd->cmd_args_field = request_args;
	
	return cmd;
}

struct s_cmd * new_stor(char * args)
{
	struct s_cmd * cmd;
	char * request_args = NULL;
	
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
		
	cmd->cmd_t = CMD_STOR;
	cmd->cmd_h = process_stor;
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
    char buf[BUF_SIZE + 1];

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
    char buf[BUF_SIZE + 1];
    char* addr[NB_AGRS_PORT];
    unsigned i;
    unsigned short x, y, port;

    if(cmd->cmd_args_field == NULL){
	/* Syntax error in parameters or arguments. */
	snprintf(buf, BUF_SIZE, "501 Syntax error in parameters or arguments.\r\n");

	if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	    perror("Erreur write: ");
	    return;
	}
    }

    addr[0] = strtok(cmd->cmd_args_field, ",");
    if(addr[0] == NULL){
	/* Syntax error in parameters or arguments. */
	snprintf(buf, BUF_SIZE, "501 Syntax error in parameters or arguments.\r\n");
	
	if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	    perror("Erreur write: ");
	    return;
	}
    }

    for(i=1; i<NB_AGRS_PORT; i++){
	addr[i] = strtok(NULL, ",");
	if(addr[i] == NULL){
	    /* Syntax error in parameters or arguments. */
	    snprintf(buf, BUF_SIZE, "501 Syntax error in parameters or arguments.\r\n");
	
	    if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
		perror("Erreur write: ");
		return;
	    }
	}
    }
    x = atoi(addr[4]);
    y = atoi(addr[5]);
    port = x*256 + y;
    set_port(cmd->cmd_client->cli_data_connection, port);
    printf("%d * 256 + %d = %d\n", x, y, port);

    /* Command okay. */
    snprintf(buf, BUF_SIZE, "200 Command okay.\r\n");
    
    if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	perror("Erreur write: ");
	return;
    }

    return;
}

void process_list(struct s_cmd * cmd)
{
    DIR *pDir;
    struct dirent *pDirent;
    char buf[BUF_SIZE + 1];
    char bufData[BUF_SIZE + 1];
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

	    if(strncpy(bufData, pDirent->d_name, BUF_SIZE) == NULL){
		perror("Erreur strncpy: ");
		status = -1;
	    }
	    
	    if(strcat(bufData, "  ")== NULL){
		perror("Erreur strcat: ");
		status = -1;
	    }
	
	    while ((pDirent = readdir(pDir)) != NULL) {
		if(strcat(bufData, pDirent->d_name)== NULL){
		    perror("Erreur strcat: ");
		    status = -1;
		    break;
		}
		if(strcat(bufData, "  ")== NULL){
		    perror("Erreur strcat: ");
		    status = -1;
		    break;
		}
	    }

	    strcat(bufData, "\r\n");

	} else {
	    bufData[0] = '\0';
	}
    }

    if(status>=0){

	/* SEND LIST*/
	c_data_connect = client->cli_data_connection;
	if(c_data_connect == NULL){
	    fprintf(stderr, "Erreur: champ cli_data_connection null.\n");
	    status = -1;
	}

	if(c_data_connect->dc_transfer_t == DT_ACTIVE){
		
	    if (!is_data_connection_opened(c_data_connect)){
		
		if(open_data_connection(c_data_connect) == -1){
		    fprintf(stderr, "Erreur: Ouverture connection data.\n");
		    status = -3;
		}

		/* File status okay; about to open data connection. */
		snprintf(buf, BUF_SIZE, "150 File status okay; about to open data connection.\r\n");
	
		if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
		    perror("Erreur write: ");
		    return;
		}
	    } else {
		/* Data connection already open; transfer starting. */
		snprintf(buf, BUF_SIZE, "125 Data connection already open; transfer starting.\r\n");
	
		if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
		    perror("Erreur write: ");
		    return;
		}
	    }

	    write_data(bufData, c_data_connect);
	    printf("--- buf envoyé: %s\n", bufData);

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

    if(status == -1){
	/* Requested action aborted: local error in processing. */
	snprintf(buf, BUF_SIZE, "451 Requested action aborted: local error in processing.\r\n");

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
    } else if(status == -3){
	/* Can't open data connection. */
	snprintf(buf, BUF_SIZE, "425 Can\'t open data connection.\r\n");

	if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
	    perror("Erreur write: ");
	    return;
	}
    }

    return;
}

void process_retr(struct s_cmd * cmd)
{
	struct s_data_connection * dc;
	int sock_user_PI;
	char path[PATHNAME_MAXLEN];
	ssize_t ret;
	
	/* ARGUMENT */
    if(strcpy(path, cmd->cmd_client->cli_current_path) == NULL){
	perror("Erreur strcpy: ");
    }
    if(cmd->cmd_args_field != NULL){
	if(strcat(path, "/") == NULL){
	    perror("Erreur strcat: ");
	}
	if(strcat(path, cmd->cmd_args_field) == NULL){
	    perror("Erreur strcat: ");
	}
    }
	
	sock_user_PI = cmd->cmd_client->cli_sock;
	dc = cmd->cmd_client->cli_data_connection;
	if (!is_data_connection_opened(dc))
	{
		write_socket(sock_user_PI, "150 About to open data connection.\r\n");
		if (open_data_connection(dc) < 0)
		{
			write_socket(sock_user_PI, "425 Can't open data connection.\r\n");
			fprintf(stderr, "Erreur process_retr (open_data).\n");
			return;
		}
	}
	else
	{
		write_socket(sock_user_PI, "125 Data connection already open; transfer starting.\r\n");		
	}
	
	
	ret = send_file(path, dc);
	
	if (ret == -2)
	{
		write_socket(sock_user_PI, "552 Requested file action aborted.\r\n");
		return;
	}
	else if (ret < 0)
	{
		close_data_connection(dc);
		write_socket(sock_user_PI, "426 Connection closed; transfer aborted.\r\n");
		return;
	}
	
	write_socket(sock_user_PI, "226 Closing data connection.\r\n");
	close_data_connection(dc);
	
	return;
}

void process_stor(struct s_cmd * cmd)
{
	struct s_data_connection * dc;
	int sock_user_PI;
	char path[PATHNAME_MAXLEN];
	ssize_t ret;
	
	/* ARGUMENT */
    if(strcpy(path, cmd->cmd_client->cli_current_path) == NULL){
	perror("Erreur strcpy: ");
    }
    if(cmd->cmd_args_field != NULL){
	if(strcat(path, "/") == NULL){
	    perror("Erreur strcat: ");
	}
	if(strcat(path, cmd->cmd_args_field) == NULL){
	    perror("Erreur strcat: ");
	}
    }
	
	sock_user_PI = cmd->cmd_client->cli_sock;
	dc = cmd->cmd_client->cli_data_connection;
	if (!is_data_connection_opened(dc))
	{
		write_socket(sock_user_PI, "150 About to open data connection.\r\n");
		if (open_data_connection(dc) < 0)
		{
			write_socket(sock_user_PI, "425 Can't open data connection.\r\n");
			fprintf(stderr, "Erreur process_retr (open_data).\n");
			return;
		}
	}
	else
	{
		write_socket(sock_user_PI, "125 Data connection already open; transfer starting.\r\n");		
	}
	
	
	ret = read_file(path, dc);
	
	if (ret == -2)
	{
		write_socket(sock_user_PI, "552 Requested file action aborted.\r\n");
		return;
	}
	else if (ret < 0)
	{
		close_data_connection(dc);
		write_socket(sock_user_PI, "426 Connection closed; transfer aborted.\r\n");
		return;
	}
	
	write_socket(sock_user_PI, "226 Closing data connection.\r\n");
	close_data_connection(dc);
	
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
