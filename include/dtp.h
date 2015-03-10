#ifndef DTP_H_
#define DTP_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/**
 * @file dtp.h
 * @brief Contient la structure et les fonctions permettant de gérer la
 * connexion de données FTP avec le client.
 */

/** Type de transfert de données : passif ou non. */
enum data_transfer_type {DT_ACTIVE, DT_PASSIVE};

struct s_data_connection
{
    /** 
     * Socket de transert de données. Vaut -1 si pas de connexion
     * ouverte 
     */
    int dc_socket;
    /** Adresse du client pour le transfert de données (avec le port) */
    struct sockaddr_in dc_addr;
    /** Type de transfert de données */
    enum data_transfer_type dc_transfer_t;
};

/**
 * Initialise la structure data connection avec l'adresse passée en
 * paramètre.
 * @param[in] data_addr l'adresse du client.
 * 
 * @return un pointeur vers la structure nouvellement créée.
 *  
 */
struct s_data_connection * new_data_connection(struct sockaddr_in data_addr);

/**
 * Ouvre la connexion de données avec le client selon les paramètres définis dans
 * la structure.
 * @param[in] dc un pointeur vers la structure à utiliser.
 * 
 * @return -1 en cas d'erreur, 1 sinon.
 *  
 */
int open_data_connection(struct s_data_connection * dc);

/**
 * Ferme la connexion de données avec le client.
 * @param[in] dc un pointeur vers la structure à utiliser.
 * 
 * @return -1 en cas d'erreur, 0 sinon.
 * 
 */
int close_data_connection(struct s_data_connection * dc);

/** 
 * Modifie la valeur du port à utiliser.
 * @param[in] dc un pointeur vers la structure à utiliser.
 * @param[in] port la nouvelle valeur du port.
 *  
 */
void set_port(struct s_data_connection * dc, unsigned short port);

/**
 * Fixe le type de connexion au type actif.
 * @param[in] dc un pointeur vers la structure à modifier.
 *  
 */
void set_transfer_t_active(struct s_data_connection * dc);

/** Fixe le type de connexion au type passif.
 * @param[in] dc un pointeur vers la structure à modifier.
 *  */
void set_transfer_t_passive(struct s_data_connection * dc);

/** Fixe le type de connexion au type actif.
 * @param[in] dc un pointeur vers la structure.
 * 
 * @return vrai (1) si la connexion est ouverte, faux (0) sinon.
 *  */
int is_data_connection_opened(struct s_data_connection * dc);

/**
 * Envoie le contenu d'un buffer via la connexion de données.
 * @param[in] dc un pointeur vers la structure de la connexion.
 * @param[in] message le buffer à envoyer.
 * 
 * @return le nombre d'octets envoyés et -1 en cas d'erreur
 * 
 */
ssize_t write_data(char * message, struct s_data_connection * dc);

/**
 * Crée un fichier et y inscrit le contenu d'un fichier lu via la 
 * connexion de données.
 * @param[in] dc un pointeur vers la structure de la connexion.
 * @param[in] pathname le chemin du fichier à créer.
 * 
 * @return le nombre d'octets lus ou en cas d'erreur :
 * 			-1 si pas de connexion ouverte<br/>
 * 			-2 si la création échoue<br/>
 * 			-3 si problème de lecture<br/>
 * 			-4 si problème d'écriture<br/>
 */
ssize_t read_file(char * pathname, struct s_data_connection * dc);

/**
 * Lit un fichier et en envoie le contenu via la connexion de données.
 * @param[in] dc un pointeur vers la structure de la connexion.
 * @param[in] pathname le chemin du fichier à envoyer.
 * 
 * @return le nombre d'octets envoyés ou en cas d'erreur :
 * 			-1 si pas de connexion ouverte<br/>
 * 			-2 si la création échoue<br/>
 * 			-3 si problème de lecture<br/>
 * 			-4 si problème d'écriture<br/>
 * 			-5 si autre problème<br/>
 */
ssize_t send_file(char * pathname, struct s_data_connection * dc);


#endif /* DTP_H_ */
