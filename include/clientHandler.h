#ifndef CMD_HANDLER_H_
#define CMD_HANDLER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_PATH_LEN 4096

/**
 * @file cmdHandler.h
 * @brief Gére les commandes provenant du client
 */

struct s_client
{
	char cli_current_path[4096];
	int cli_sock;
	struct sockaddr_in cli_addr;
	unsigned short cli_data_port;
};

void handle_client(struct sockaddr_in client_addr, int socket);

#endif /* CMD_HANDLER_H_ */
