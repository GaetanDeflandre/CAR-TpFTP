#ifndef CLIENT_HANDLER_H_
#define CLIENT_HANDLER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "database.h"
#include "dtp.h"

/**
 * @file clientHandler.h
 * @brief Gére les commandes provenant du client
 */

/** 
 * Structure représentant les paramètres d'une connexion avec un 
 * client. 
 */
struct s_client
{
	/** Nom de l'utilisateur, si connecté. */
	char * cli_username;
	/** Vaut 1 si le client est authentifié, 0 sinon. */
	int cli_logged_in;
    /** Chemin courant du client */
    char cli_current_path[PATHNAME_MAXLEN];
    /** Socket du client */
    int cli_sock;
    /** Adresse du client */
    struct sockaddr_in cli_addr;
    /** Paramètres de transfert de données du client */
    struct s_data_connection * cli_data_connection;
};

/** 
 * Interprète les requêtes d'un client et exécute les commandes 
 * appropriées jusqu'à la fin de la connexion. 
 */
void handle_client(struct sockaddr_in client_addr, int socket);

/**
 * Ecrit le ocntenu d'un buffer dans le socket du client.
 *
 * @param[in] socket Le socket du client.
 * @param[in] buf le buffer à envoyer.
 * @return le nombre d'octets envoyés ou
 *         -1 en cas d'erreur.<br\>
 */
ssize_t write_client(int socket, char * buf);
/**
 * Termine la connection avec le client.
 * @param[in] client le client avec lequel la connection est fermée.
 */
void close_connection(struct s_client * client);

#endif /* CLIENT_HANDLER_H_ */
