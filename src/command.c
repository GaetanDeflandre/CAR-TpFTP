#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
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
#define CODE_FEAT "FEAT"
#define CODE_PORT "PORT"
#define CODE_LIST "LIST"
#define CODE_PWD  "PWD"
#define CODE_CWD  "CWD"
#define CODE_RMD  "RMD"
#define CODE_MKD  "MKD"
#define CODE_DELE "DELE"
#define CODE_RETR "RETR"
#define CODE_STOR "STOR"
#define CODE_TYPE "TYPE"

/* CREATION DE COMMANDES */
struct s_cmd * new_user(char * args);
struct s_cmd * new_pass(char * args);
struct s_cmd * new_quit(char * args);
struct s_cmd * new_syst(char * args);
struct s_cmd * new_feat(char * args);
struct s_cmd * new_port(char * args);
struct s_cmd * new_list(char * args);
struct s_cmd * new_pwd(char * args);
struct s_cmd * new_cwd(char * args);
struct s_cmd * new_rmd(char * args);
struct s_cmd * new_mkd(char * args);
struct s_cmd * new_dele(char * args);
struct s_cmd * new_retr(char * args);
struct s_cmd * new_stor(char * args);
struct s_cmd * new_type(char * args);

/* HANDLERS DES COMMANDES */
void process_user(struct s_cmd * cmd);
void process_pass(struct s_cmd * cmd);
void process_quit(struct s_cmd * cmd);
void process_syst(struct s_cmd * cmd);
void process_feat(struct s_cmd * cmd);
void process_port(struct s_cmd * cmd);
void process_list(struct s_cmd * cmd);
void process_pwd(struct s_cmd * cmd);
void process_cwd(struct s_cmd * cmd);
void process_rmd(struct s_cmd * cmd);
void process_mkd(struct s_cmd * cmd);
void process_dele(struct s_cmd * cmd);
void process_retr(struct s_cmd * cmd);
void process_stor(struct s_cmd * cmd);
void process_type(struct s_cmd * cmd);

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
	else if (strncasecmp(request_code, CODE_FEAT, strlen(request_code)) == 0)
	{
		cmd = new_feat(request_args);
	}
	else if (strncasecmp(request_code, CODE_PORT, strlen(request_code)) == 0)
	{
		cmd = new_port(request_args);
	}
	else if (strncasecmp(request_code, CODE_LIST, strlen(request_code)) == 0)
	{
		cmd = new_list(request_args);
	}
	else if (strncasecmp(request_code, CODE_PWD, strlen(request_code)) == 0)
	{
		cmd = new_pwd(request_args);
	}
	else if (strncasecmp(request_code, CODE_CWD, strlen(request_code)) == 0)
	{
		cmd = new_cwd(request_args);
	}
	else if (strncasecmp(request_code, CODE_DELE, strlen(request_code)) == 0)
	{
		cmd = new_dele(request_args);
	}
	else if (strncasecmp(request_code, CODE_RMD, strlen(request_code)) == 0)
	{
			cmd = new_rmd(request_args);
	}
	else if (strncasecmp(request_code, CODE_MKD, strlen(request_code)) == 0)
	{
		cmd = new_mkd(request_args);
	}
	else if (strncasecmp(request_code, CODE_RETR, strlen(request_code)) == 0)
	{
		cmd = new_retr(request_args);
	}
	else if (strncasecmp(request_code, CODE_STOR, strlen(request_code)) == 0)
	{
		cmd = new_stor(request_args);
	} 
	else if (strncasecmp(request_code, CODE_TYPE, strlen(request_code)) == 0)
	{
		cmd = new_type(request_args);
	} 
	else 
	{
        cmd = NULL;
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

struct s_cmd * new_feat(char * args)
{
    struct s_cmd * cmd;
    char * request_args = NULL;

    cmd = malloc(sizeof(struct s_cmd));
    if (cmd == NULL){
        fprintf(stderr, "Erreur new_feat: Erreur d'allocation de memoire.\n");
        return NULL;
    }

    if (args != NULL){
        request_args = malloc((strlen(args) + 1) * sizeof(char));
        strcpy(request_args, args);
    }

    cmd->cmd_t = CMD_FEAT;
    cmd->cmd_h = process_feat;
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

struct s_cmd * new_pwd(char * args)
{
    struct s_cmd * cmd;
    char * request_args = NULL;

    cmd = malloc(sizeof(struct s_cmd));
    if (cmd == NULL){
        fprintf(stderr, "Erreur new_pwd: Erreur d'allocation de memoire.\n");
        return NULL;
    }

    if (args != NULL){
        request_args = malloc((strlen(args) + 1) * sizeof(char));
        strcpy(request_args, args);
    }

    cmd->cmd_t = CMD_PWD;
    cmd->cmd_h = process_pwd;
    cmd->cmd_args_field = request_args;
        
    return cmd;
}

struct s_cmd * new_cwd(char * args)
{
    struct s_cmd * cmd;
    char * request_args = NULL;

    cmd = malloc(sizeof(struct s_cmd));
    if (cmd == NULL){
        fprintf(stderr, "Erreur new_cwd: Erreur d'allocation de memoire.\n");
        return NULL;
    }

    if (args != NULL){
        request_args = malloc((strlen(args) + 1) * sizeof(char));
        strcpy(request_args, args);
    }

    cmd->cmd_t = CMD_CWD;
    cmd->cmd_h = process_cwd;
    cmd->cmd_args_field = request_args;
        
    return cmd;
}

struct s_cmd * new_dele(char * args)
{
    struct s_cmd * cmd;
    char * request_args = NULL;

    cmd = malloc(sizeof(struct s_cmd));
    if (cmd == NULL){
        fprintf(stderr, "Erreur new_dele: Erreur d'allocation de memoire.\n");
        return NULL;
    }

    if (args != NULL){
        request_args = malloc((strlen(args) + 1) * sizeof(char));
        strcpy(request_args, args);
    }

    cmd->cmd_t = CMD_DELE;
    cmd->cmd_h = process_dele;
    cmd->cmd_args_field = request_args;
        
    return cmd;
}

struct s_cmd * new_rmd(char * args)
{
    struct s_cmd * cmd;
    char * request_args = NULL;

    cmd = malloc(sizeof(struct s_cmd));
    if (cmd == NULL){
        fprintf(stderr, "Erreur new_rmd: Erreur d'allocation de memoire.\n");
        return NULL;
    }

    if (args != NULL){
        request_args = malloc((strlen(args) + 1) * sizeof(char));
        strcpy(request_args, args);
    }

    cmd->cmd_t = CMD_RMD;
    cmd->cmd_h = process_rmd;
    cmd->cmd_args_field = request_args;
        
    return cmd;
}

struct s_cmd * new_mkd(char * args)
{
    struct s_cmd * cmd;
    char * request_args = NULL;

    cmd = malloc(sizeof(struct s_cmd));
    if (cmd == NULL){
        fprintf(stderr, "Erreur new_mkd: Erreur d'allocation de memoire.\n");
        return NULL;
    }

    if (args != NULL){
        request_args = malloc((strlen(args) + 1) * sizeof(char));
        strcpy(request_args, args);
    }

    cmd->cmd_t = CMD_MKD;
    cmd->cmd_h = process_mkd;
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

struct s_cmd * new_type(char * args)
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
			
	cmd->cmd_t = CMD_TYPE;
	cmd->cmd_h = process_type;
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
			strncpy(cmd->cmd_client->cli_root_path, path, PATHNAME_MAXLEN);
                        strncpy(cmd->cmd_client->cli_current_path, path, PATHNAME_MAXLEN);
                        snprintf(buf, BUF_SIZE, "230 User logged in, proceed.\r\n");
                        write_socket(cmd->cmd_client->cli_sock, buf);
                }
                else
                {
                        free(cmd->cmd_client->cli_username);
			memset(cmd->cmd_client->cli_root_path, 0, PATHNAME_MAXLEN);
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
    snprintf(buf, BUF_SIZE, "215 unix\r\n");
    
    if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
        perror("Erreur write: ");
        return;
    }
    return;
}

void process_feat(struct s_cmd * cmd)
{
    not_implemented_command(cmd->cmd_client);
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
	return;
    }

    addr[0] = strtok(cmd->cmd_args_field, ",");
    if(addr[0] == NULL){
        /* Syntax error in parameters or arguments. */
        snprintf(buf, BUF_SIZE, "501 Syntax error in parameters or arguments.\r\n");
        
        if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
            perror("Erreur write: ");
            return;
        }
	return;
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
	    return;
        }
    }
    x = atoi(addr[4]);
    y = atoi(addr[5]);
    port = x*256 + y;
    set_port(cmd->cmd_client->cli_data_connection, port);

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
    char buf[BUF_SIZE + 1];
    char path[PATHNAME_MAXLEN];
    int status = 0;
    int child_status;
    struct s_client * client;
    struct s_data_connection * dc;
    struct stat stat_buf;
    
    client = cmd->cmd_client;
    dc = client->cli_data_connection;

    if(client == NULL){
        fprintf(stderr, "Erreur: client vaut null.\n");
        status = -1;
    }

    /* ARGUMENT */
    if(strcpy(path, client->cli_current_path) == NULL){
		perror("Erreur strcpy: ");
		status = -1;
    }
    
    if(cmd->cmd_args_field != NULL) {
		if(strcat(path, "/") == NULL){
			perror("Erreur strcat: ");
			status = -1;
		}
		if(strcat(path, cmd->cmd_args_field) == NULL){
			perror("Erreur strcat: ");
			status = -1;
		}
    }
    
    if (status >= 0 && is_valid_path(client, path))
    {
		if (lstat(path, &stat_buf)==-1)
		{
			perror("Erreur process_list (stat)");
			status = -2;
		}
	}
	else
	{
		status = -2;
	}

    if(status>=0){
		if (S_ISREG(stat_buf.st_mode) || S_ISDIR(stat_buf.st_mode))
		{
			if (!is_data_connection_opened(dc))
			{
				write_socket(client->cli_sock, "150 About to open data connection.\r\n");
				if (open_data_connection(dc) < 0)
				{
					write_socket(client->cli_sock, "425 Can't open data connection.\r\n");
					fprintf(stderr, "Erreur process_list (open_data).\n");
					return;
				}
			}
			else
			{
				write_socket(client->cli_sock, "125 Data connection already open; transfer starting.\r\n");		
			}
			
			switch (fork())
			{
				case -1:
					perror("Erreur process_list (fork)");
					status = -1;
					break;
				case 0:
					close(1);
					close(client->cli_sock);
					if (dup(dc->dc_socket) ==-1)
					{
						perror("Erreur process_list (dup)");
						status = -1;
					}
					else
					{
						if (execlp("ls", "ls", "-l", path, NULL) == -1)
							exit(EXIT_FAILURE);
					}
					break;
				default:
					if (wait(&child_status) == -1)
					{
						perror("Erreur process_list (wait)");
						status = -1;
					}
					else
					{
						if (WIFEXITED(child_status) && WEXITSTATUS(child_status) == EXIT_SUCCESS)
						{
							/* Can't open data connection. */
							snprintf(buf, BUF_SIZE, "226 Closing data connection.\r\n");
							close_data_connection(dc);
							if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
								perror("Erreur write: ");
								return;
							}
						}
						else
						{
							fprintf(stderr, "Erreur process_list (ls)");
							status = -1;
						}
					}
			}
		}
		else
		{
			status = -2;
		}
    }

    if(status == -1){
		/* Requested action aborted: local error in processing. */
		snprintf(buf, BUF_SIZE, "451 Requested action aborted: local error in processing.\r\n");

		if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
			perror("Erreur write: ");
			return;
		}
    } else if(status == -2) {
		/* Syntax error in parameters or arguments. */
		snprintf(buf, BUF_SIZE, "501 Syntax error in parameters or arguments.\r\n");

		if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
			perror("Erreur write: ");
			return;
		}
	}
	
    return;
}

void process_pwd(struct s_cmd * cmd)
{
    char buf[PATHNAME_MAXLEN+1];
    struct s_client * client;
    int sock_user;

    client = cmd->cmd_client;
    sock_user = client->cli_sock;

    /* ERROR CASE */
    if(client == NULL){
	fprintf(stderr, "Erreur: client vaut NULL.\n");
	write_socket(sock_user, "421 Service not available, closing control connection.\r\n");
	return;
    }
    if( client->cli_current_path == NULL){
	fprintf(stderr, "Erreur: le champ cli_current_path vaut NULL.\n");
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	return;
    }

    /* NORMAL CASE */
    snprintf(buf, BUF_SIZE, "257 \"%s\" created.\r\n", client->cli_current_path);
    write_socket(sock_user, buf);
    return;

}

void process_cwd(struct s_cmd * cmd)
{
    int sock_user;
    char dirname[PATHNAME_MAXLEN+1];
    char path[PATHNAME_MAXLEN+1];
    char catpath[PATHNAME_MAXLEN+1];
    char* lastchr;

    sock_user = cmd->cmd_client->cli_sock;
  
    /* ARGUMENT */
    if(strcpy(path, cmd->cmd_client->cli_current_path) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
        perror("Erreur strcpy: ");
	return;
    }
    if(strcpy(catpath, path) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
        perror("Erreur strcpy: ");
	return;
    }
    if(strcpy(dirname, cmd->cmd_args_field) == NULL){
	write_socket(sock_user, "501 Syntax error in parameters or arguments.\r\n");
        perror("Erreur strcpy: ");
	return;
    }
    if(strcat(catpath, "/") == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	perror("Erreur strcat: ");
	return;
    }
    if(strcat(catpath, dirname) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	perror("Erreur strcat: ");
	return;
    }

    if(!is_dir_of_currentpath(cmd->cmd_client, dirname)){
	write_socket(sock_user, "501 Syntax error in parameters or arguments.\r\n");
	return;
    }

    /* . CASE */
    if(strcmp(dirname, ".") == 0){
	write_socket(sock_user, "250 Requested file action okay, completed.\r\n");
	return;
    } 

    /* .. CASE */
    if (strcmp(dirname, "..") == 0) {
	if(strcmp(path, cmd->cmd_client->cli_root_path) == 0){
	    write_socket(sock_user, "501 Syntax error in parameters or arguments.\r\n");
	    return;
	} 

	lastchr = strrchr(path, '/');
	int id = lastchr-path;
	path[id] = '\0';
	strcpy(cmd->cmd_client->cli_current_path, path);
	write_socket(sock_user, "250 Requested file action okay, completed.\r\n");
	return;
    } 

    /* NORMAL CASE */
    strcpy(cmd->cmd_client->cli_current_path, catpath);
    write_socket(sock_user, "250 Requested file action okay, completed.\r\n");
    return;
  

}

void process_dele(struct s_cmd * cmd)
{
    int sock_user;
    char filename[PATHNAME_MAXLEN+1];
    char path[PATHNAME_MAXLEN+1];
    char catpath[PATHNAME_MAXLEN+1];

    sock_user = cmd->cmd_client->cli_sock;
  
    /* ARGUMENT */
    if(strcpy(filename, cmd->cmd_args_field) == NULL){
	write_socket(sock_user, "501 Syntax error in parameters or arguments.\r\n");
        perror("Erreur strcpy");
	return;
    }

    /* CPY */
    if(strcpy(path, cmd->cmd_client->cli_current_path) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
        perror("Erreur strcpy");
	return;
    }

    /* CREATE CONCAT PATH */
    if(strcpy(catpath, path) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
        perror("Erreur strcpy");
	return;
    }
    if(strcat(catpath, "/") == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	perror("Erreur strcat");
	return;
    }
    if(strcat(catpath, filename) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	perror("Erreur strcat");
	return;
    }

    /* IS FILE OF CURRENT PATH CHECKING */
    if(!is_file_of_currentpath(cmd->cmd_client, filename)){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	fprintf(stderr, "Erreur is_file_of_currentpath: %s inexistant dans le répertoire courant.\n", filename);
	return;
    }

    /* DELETE FILE */
    if(unlink(catpath) == -1){
	perror("Erreur remove");
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	return;
    }
    /* ALL IS OK, FILE DELETED */
    printf("File %s was deleted.\n", filename);
    write_socket(sock_user, "250 Requested file action okay, completed.\r\n");
    return;
}

void process_rmd(struct s_cmd * cmd)
{
    int sock_user;
    char dirname[PATHNAME_MAXLEN+1];
    char path[PATHNAME_MAXLEN+1];
    char catpath[PATHNAME_MAXLEN+1];

    sock_user = cmd->cmd_client->cli_sock;
  
    /* ARGUMENT */
    if(strcpy(dirname, cmd->cmd_args_field) == NULL){
	write_socket(sock_user, "501 Syntax error in parameters or arguments.\r\n");
        perror("Erreur strcpy");
	return;
    }

    /* CPY */
    if(strcpy(path, cmd->cmd_client->cli_current_path) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
        perror("Erreur strcpy");
	return;
    }

    /* CREATE CONCAT PATH */
    if(strcpy(catpath, path) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
        perror("Erreur strcpy");
	return;
    }
    if(strcat(catpath, "/") == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	perror("Erreur strcat");
	return;
    }
    if(strcat(catpath, dirname) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	perror("Erreur strcat");
	return;
    }

    /* IS FILE OF CURRENT PATH CHECKING */
    if(!is_dir_of_currentpath(cmd->cmd_client, dirname)){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	fprintf(stderr, "Erreur is_dir_of_currentpath: %s inexistant dans le répertoire courant.\n", dirname);
	return;
    }

    /* REMOVE DIR */
    if(rmdir(catpath) == -1){
	perror("Erreur remove");
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	return;
    }
    /* ALL IS OK, DIR REMOVED */
    printf("File %s was removed.\n", dirname);
    write_socket(sock_user, "250 Requested file action okay, completed.\r\n");
    return;
}

void process_mkd(struct s_cmd * cmd)
{
    int sock_user;
    char dirname[PATHNAME_MAXLEN+1];
    char path[PATHNAME_MAXLEN+1];
    char catpath[PATHNAME_MAXLEN+1];
    char buf[PATHNAME_MAXLEN+1];

    sock_user = cmd->cmd_client->cli_sock;
  
    /* ARGUMENT */
    if(strcpy(dirname, cmd->cmd_args_field) == NULL){
	write_socket(sock_user, "501 Syntax error in parameters or arguments.\r\n");
        perror("Erreur strcpy");
	return;
    }

    /* CPY */
    if(strcpy(path, cmd->cmd_client->cli_current_path) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
        perror("Erreur strcpy");
	return;
    }

    /* CREATE CONCAT PATH */
    if(strcpy(catpath, path) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
        perror("Erreur strcpy");
	return;
    }
    if(strcat(catpath, "/") == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	perror("Erreur strcat");
	return;
    }
    if(strcat(catpath, dirname) == NULL){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	perror("Erreur strcat");
	return;
    }

    /* CHECK IF THE DIR ALREADY EXISTE */
    if(is_dir_of_currentpath(cmd->cmd_client, dirname)){
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	fprintf(stderr, "Erreur is_dir_of_currentpath: %s existe deja dans le répertoire courant.\n", dirname);
	return;
    }

    /* CREATE DIR */
    if(mkdir(catpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
	perror("Erreur mkdir");
	write_socket(sock_user, "550 Requested action not taken.\r\n");
	return;
    }

    /* ALL IS OK, DIR CREATED */
    printf("File %s was created.\n", dirname);
    snprintf(buf, BUF_SIZE, "257 \"%s\" created.\r\n", dirname);
    write_socket(sock_user, buf);
    return;
}

void process_retr(struct s_cmd * cmd)
{
    struct s_data_connection * dc;
    int sock_user_PI;
    char path[PATHNAME_MAXLEN];
    ssize_t ret;
    
    sock_user_PI = cmd->cmd_client->cli_sock;
    
    /* ARGUMENT */
    if(strcpy(path, cmd->cmd_client->cli_current_path) == NULL)
    {
        perror("Erreur strcpy");
        write_socket(sock_user_PI, "451 Requested action aborted: local error in processing.\r\n");
		return;
    }
    
    if(cmd->cmd_args_field != NULL){
        if(strcat(path, "/") == NULL)
        {
            perror("Erreur strcat");
            write_socket(sock_user_PI, "451 Requested action aborted: local error in processing.\r\n");
			return;
        }
        
        if(strcat(path, cmd->cmd_args_field) == NULL)
        {
            perror("Erreur strcat");
            write_socket(sock_user_PI, "451 Requested action aborted: local error in processing.\r\n");
			return;
        }
    }
    
    if (is_valid_path(cmd->cmd_client, path))
    {
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
			
		if (dc->dc_transfer_t == TYPE_ASCII)
			ret = send_ascii_file(path, dc);
		else
			ret = send_binary_file(path, dc);
			
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
	}
	else
	{
		write_socket(sock_user_PI, "550 Requested file action not taken (no access).\r\n");
		return;
	}
        
    return;
}

void process_stor(struct s_cmd * cmd)
{
    struct s_data_connection * dc;
    int sock_user_PI;
    char path[PATHNAME_MAXLEN];
    char path_control[PATHNAME_MAXLEN];
    char * p_control;
    ssize_t ret;
    
    sock_user_PI = cmd->cmd_client->cli_sock;
        
    /* ARGUMENT */
    if(strcpy(path, cmd->cmd_client->cli_current_path) == NULL){
        perror("Erreur strcpy: ");
        write_socket(sock_user_PI, "451 Requested action aborted: local error in processing.\r\n");
		return;
    }
    if(cmd->cmd_args_field != NULL){
        if(strcat(path, "/") == NULL){
            perror("Erreur strcat: ");
            write_socket(sock_user_PI, "451 Requested action aborted: local error in processing.\r\n");
			return;
        }
        
        if(strcat(path, cmd->cmd_args_field) == NULL){
            perror("Erreur strcat: ");
            write_socket(sock_user_PI, "451 Requested action aborted: local error in processing.\r\n");
			return;
        }
    }
    
    if(strcpy(path_control, path) == NULL)
    {
        perror("Erreur strcpy: ");
        write_socket(sock_user_PI, "451 Requested action aborted: local error in processing.\r\n");
		return;
    }
    
	/* Construction du chemin du répertoire qui va contenir le 
	nouveau fichier. */
    p_control = strrchr(path_control, '/');
    if (p_control != NULL)
		*p_control = '\0';
		
	if (is_valid_path(cmd->cmd_client, path_control))
	{
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
			
		if (dc->dc_transfer_t == TYPE_ASCII)
			ret = read_ascii_file(path, dc);
		else
			ret = read_binary_file(path, dc);
			
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
	}
	else
	{
		write_socket(sock_user_PI, "550 Requested file action not taken.\r\n");
	}
        
    return;
}

void process_type(struct s_cmd * cmd)
{
	struct s_client * client;
	int sock_user_PI;
	char * type_arg;
	enum data_transfer_type new_type;
	
	client = cmd->cmd_client;
	sock_user_PI = client->cli_sock;

    if(cmd->cmd_args_field == NULL)
    {
        write_socket(sock_user_PI, "501 Syntax error in parameters or arguments.\r\n");
		return;
    }
    
    type_arg = strtok(cmd->cmd_args_field, " \f\n\r\t\v");
    
    if (strncasecmp(type_arg, "A", strlen(type_arg)) == 0)
	{
		new_type = TYPE_ASCII;
	}
	else if (strncasecmp(type_arg, "I", strlen(type_arg)) == 0)
	{
		new_type = TYPE_BINARY;
	}
	else
	{
		write_socket(sock_user_PI, "501 Syntax error in parameters or arguments.\r\n");
		return;
	}
    
    if (strtok(NULL, " \f\n\r\t\v") != NULL)
    {
		write_socket(sock_user_PI, "504 Command not implemented for that parameter.\r\n");
		return;
	}

    /* Command okay. */
    write_socket(sock_user_PI, "200 Command okay.\r\n");
    
    if (new_type == TYPE_ASCII)
		set_transfer_t_ascii(client->cli_data_connection);
	else if (new_type == TYPE_BINARY)
		set_transfer_t_binary(client->cli_data_connection);
		
    return;
}

void destroy_cmd(struct s_cmd * cmd)
{
    if (cmd != NULL) {
	free(cmd->cmd_args_field);
	free(cmd);
    }
}
