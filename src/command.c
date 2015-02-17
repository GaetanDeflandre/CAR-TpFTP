#include <command.h>
#include <string.h>
#include <cmdUser.h>

#define CODE_LENGTH 4
#define CODE_USER "USER"

struct s_cmd * init_cmd(char * client_request)
{
	struct s_cmd * cmd;
	char *request_code, *request_args;
	char request_line[MAX_LINE_LENGTH];
	
	strncpy(request_line, client_request, MAX_LINE_LENGTH);
	
	request_code = strtok(request_line, " ");
	request_args = strtok(NULL, "\r\n");
	
	if (strncmp(request_code, CODE_USER, CODE_LENGTH) == 0)
	{
		cmd = new_user(request_args);
	}
	else
	{
		return NULL;
	}
	
	return cmd;
}

void exec_cmd(struct s_cmd * cmd)
{
	cmd->cmd_h(cmd->cmd_args);
}
