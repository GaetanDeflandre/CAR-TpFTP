#ifndef SERV_FTP_H_
#define SERV_FTP_H_

/**
 * @file servFTP.h
 * @brief Etablit une connection avec un client
 */

/** Port sur lequel le serveur écoute. */
#define SERV_PORT 2121
/** Taille des messages qui servent à établir la connection. */
#define BUF_SIZE 512


/**
 * Lance le serveur. Le serveur écoute en boucle sur son port 
 * de communication.
 * @return 0 en cas de fonctionnement correct, -1 sinon. 
 */
int launch_server();

#endif /* SERV_FTP_H_ */
