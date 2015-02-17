#ifndef DATABASE_H_
#define DATABASE_H_

/**
 * @file database.h
 * @brief Charge la base utilisateur en mémoire et gère 
 * la base.
 */

#include <libxml/tree.h>
#include <libxml/parser.h>

/** Chemin vers la base de données */
#define DATABASE_FILE "etc/database.xml"
/** Répertoire des fichiers des utilisateurs */
#define FILES_DIR "files/"
/** Longueur maximum pour un mot de passe */
#define PASSWORD_MAXLEN 126
/** Longueur maximum pour le nom de répertoire d'un utilisateur */
#define PATHNAME_MAXLEN 1024

/**
 * Recherche l'utilisateur \a name dans la base de donnée.
 *
 * @param[in] name Le nom de l'utilisateur à rechercher.
 * @param[out] password Est copié le mot de passe si l'utilisateur 
 * recherché s'il existe.
 * @param[out] path Est copié le nom du répertoire de fichiers de 
 * l'utilisateur recherché, s'il existe.
 * @return 1 si l'utilisateur est trouvé.<br\>
 *         0 si l'utilisateur n'est pas trouvé.<br\>
 *         -1 en cas d'erreur.<br\>
 */
int get_user_info(const char* name, char** password, char** path);

#endif /* DATABASE_H_ */
