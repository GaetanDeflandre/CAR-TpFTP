#ifndef COMMAND_H_
#define COMMAND_H_

#include "clientHandler.h"

/** Taille maximum d'une ligne: 1024 + 1 pour le '\0' */
#define MAX_LINE_LENGTH 1025

/**
 * @file command.h
 * @brief Fonctions des commandes proposé par le serveur.
 */

/** Les types des commandes */
enum cmd_type {CMD_USER, CMD_PASS, CMD_SYST, CMD_LIST, CMD_RETR, CMD_STOR, 
	CMD_QUIT, CMD_PASV, CMD_PWD, CMD_CWD, CMD_CDUP, CMD_MKD, CMD_RMD};
/**
 * @struct s_cmd
 * Structure de la commande d'un client.
 */
struct s_cmd;

/**
 * Type de la fonction handler commande.
 */
typedef void (cmd_handler)(struct s_cmd *);


struct s_cmd
{
    /** Fonction à appeler */
    cmd_handler * cmd_h;
    /** Arguments de la commande */
    char * cmd_args_field;
    /** Type de la commande */
    enum cmd_type cmd_t;
    /** Client à l'origine de la commande */
    struct s_client * cmd_client;
};


/**
 * Initialise la commande du client.
 * @param[in] client_request La requête, commande demandé par le client.
 * @param[in] client Le client qui établit la requête.
 * @return Un pointeur sur la structure de la commande, NULL en 
 * cas d'erreur.
 */
struct s_cmd * init_cmd(char * client_request, struct s_client * client);

/**
 * Execute la commande \a cmd en paramètre.
 * @param[in] cmd La commande à exécuter
 */
void exec_cmd(struct s_cmd * cmd);

#endif /* COMMAND_H_ */
