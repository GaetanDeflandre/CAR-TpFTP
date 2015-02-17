#include "cmdUser.h"
#include <command.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void process_user(char * args);

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

void process_user(char * args)
{
	printf("Ok %s\n", args);
}
