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
    /** Chemin de la racine du client */
    char cli_root_path[PATHNAME_MAXLEN];
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
 * Retourne vrai si le chemin \a path est un fichier ou répertoire du
 * client \a client.
 */
unsigned is_valid_path(struct s_client * client, const char* path);

/**
 * Retourne vrai si le membre de nom \a membername est dans le
 * répertoire courrent du client \a client.
 */
unsigned is_member_of_currentpath(struct s_client * client, const char* membername);

/**
 * Retourne vrai si le fichier de nom \a filename est un fichier dans
 * le répertoire courrent du client \a client.
 */
unsigned is_file_of_currentpath(struct s_client * client, const char* filename);

/**
 * Retourne vrai si le répertoire de nom \a dirname est un fichier
 * dans le répertoire courrent du client \a client.
 */
unsigned is_dir_of_currentpath(struct s_client * client, const char* dirname);

/**
 * Interprète les requêtes d'un client et exécute les commandes 
 * appropriées jusqu'à la fin de la connexion. 
 */
void handle_client(struct sockaddr_in client_addr, int socket);

/**
 * Termine la connection avec le client.
 * @param[in] client le client avec lequel la connection est fermée.
 */
void close_connection(struct s_client * client);

#endif /* CLIENT_HANDLER_H_ */
