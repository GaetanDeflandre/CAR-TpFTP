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
/** Longueur maximum pour un mot de passe */
#define PASSWORD_MAXLEN 126


int init_database(const char* filename);
xmlNodePtr get_root();
int login(const char* name, char** password, char** path);
int walker(const xmlNodePtr node, const char* name, char** password, char** path);

#endif /* DATABASE_H_ */
