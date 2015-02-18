#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <sys/types.h>

/**
 * Ecrit le ocntenu d'un buffer dans le socket du client.
 *
 * @param[in] socket Le socket du client.
 * @param[in] buf le buffer à envoyer.
 * @return le nombre d'octets envoyés ou
 *         -1 en cas d'erreur.<br\>
 */
ssize_t write_socket(int socket, char * buf);

#endif /* COMMUNICATION_H */
