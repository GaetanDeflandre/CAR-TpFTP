#ifndef COMMAND_H_
#define COMMAND_H_

/* 1024 + 1 pour le '\0' */
#define MAX_LINE_LENGTH 1025

/**
 * @file command.h
 * @brief Fonctions des commandes proposé par le serveur.
 */

enum cmd_type {CMD_USER, CMD_PASS, CMD_LIST, CMD_RETR, CMD_STOR, CMD_QUIT};

typedef void (cmd_handler)(char *);

struct s_cmd
{
	cmd_handler * cmd_h;
	char * cmd_args;
	enum cmd_type cmd_t;
};

struct s_cmd * init_cmd(char * client_request);
void exec_cmd(struct s_cmd * cmd);

#endif /* COMMAND_H_ */
