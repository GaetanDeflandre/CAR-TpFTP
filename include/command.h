#ifndef COMMAND_H_
#define COMMAND_H_

#include <clientHandler.h>

/* 2048 + 1 pour le '\0' */
#define MAX_REQUEST_LENGTH 2049

/**
 * @file command.h
 * @brief Fonctions des commandes proposé par le serveur.
 */

enum cmd_type {CMD_USER, CMD_PASS, CMD_LIST, CMD_RETR, CMD_STOR, 
	CMD_QUIT, CMD_PASV, CMD_PWD, CMD_CWD, CMD_CDUP, CMD_MKD, CMD_RMD};

struct s_cmd;

typedef void (cmd_handler)(struct s_cmd *);

struct s_cmd
{
	cmd_handler * cmd_h;
	char * cmd_args_field;
	enum cmd_type cmd_t;
	struct s_client * cmd_client;
};

struct s_cmd * init_cmd(char * client_request, struct s_client * client);
void exec_cmd(struct s_cmd * cmd);

#endif /* COMMAND_H_ */
