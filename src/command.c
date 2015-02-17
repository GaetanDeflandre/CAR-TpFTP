#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define CODE_LENGTH 4
#define CODE_USER "USER"

struct s_cmd * new_user(char * args);
void process_user(struct s_cmd * cmd);

struct s_cmd * init_cmd(char * client_request, struct s_client * client)
{
	struct s_cmd * cmd;
	char *request_code, *request_args;
	char request_line[MAX_LINE_LENGTH];
	
	strncpy(request_line, client_request, MAX_LINE_LENGTH);
	
	request_code = strtok(request_line, " ");
	request_args = strtok(NULL, "\r\n");
	
	if (strncasecmp(request_code, CODE_USER, CODE_LENGTH) == 0)
	{
		cmd = new_user(request_args);
	}
	else
	{
		return NULL;
	}
	
	cmd->cmd_client = client;
	return cmd;
}

void exec_cmd(struct s_cmd * cmd)
{
	cmd->cmd_h(cmd);
}

struct s_cmd * new_user(char * args)
{
	struct s_cmd * cmd;
	char * handler_args;
	
	handler_args = malloc(strlen(args) * sizeof(char) + 1);
	cmd = malloc(sizeof(struct s_cmd));
	strcpy(handler_args, args);
	
	cmd->cmd_t = CMD_USER;
	cmd->cmd_h = process_user;
	cmd->cmd_args = handler_args;
	
	return cmd;
}

void process_user(struct s_cmd * cmd)
{
	printf("Ok %s\n", cmd->cmd_args);
	strncpy(cmd->cmd_client->cli_current_path, "somewhere", MAX_PATH_LEN);
	printf("Chemin : %s\n", cmd->cmd_client->cli_current_path);
	
	return;
}
