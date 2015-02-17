#ifndef CLIENT_HANDLER_H_
#define CLIENT_HANDLER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_PATH_LEN 4096

/**
 * @file cmdHandler.h
 * @brief Gére les commandes provenant du client
 */

/** Type de transfert de données : passif ou non. */
enum data_transfer_type {NORMAL_DT, PASSIVE_DT};

/** Structure représentant les paramètres d'une connexion avec un 
 * client. */
struct s_client
{
	char cli_current_path[MAX_PATH_LEN];
	int cli_sock;
	struct sockaddr_in cli_addr;
	unsigned short cli_data_port;
	enum data_transfer_type cli_data_transfer_t;
};

/** Interprète les requêtes d'un client et exécute les commandes 
appropriées jusqu'à la fin de la connexion. */
void handle_client(struct sockaddr_in client_addr, int socket);

#endif /* CLIENT_HANDLER_H_ */
