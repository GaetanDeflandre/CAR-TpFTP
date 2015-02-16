#ifndef DATABASE_H_
#define DATABASE_H_

/**
 * @file database.h
 * @brief Charge la base utilisateur en mémoire et gère 
 * la base.
 */

#define DATABASE_FILE "etc/database.xml"

/**
 * @struct user_s 
 * @brief La paire login et mot de passe de l'utilisateur. 
 */
struct user_s{
    char* login;
    char* password;
};

/**
 * @typedef list_user_t
 * @brief Liste des utilisateurs (login et mot de passe). 
 */
typedef struct user_s* list_user_t;


/**
 * Initialise la base de données.
 *
 * @return -1 en case d'erreur, sion 0.
 */
int init_database();

/**
 * Charge la base utilisateur en mémoire.
 * 
 * @param[out] database La base de données, une liste de 
 *                      couple login et mot de passe.
 * @return -1 en case d'erreur, sion 0.
 */
int load_database(list_user_t* database);

/**
 * Retourne vrai si l'utilisateur <code>login</code> pour 
 * mot de passe <code>password</code> est dans la base et 
 * que son mot de passe est correct.
 *
 * @param[in] login Le nom de l'utilisateur.
 * @param[in] password Le mot de passe de l'utilisateur.
 * @return 1 si l'utilisateur <code>login</code> pour 
 * mot de passe <code>password</code> est dans la base et 
 * que son mot de passe est correct, sinon retourne 0.
 */
unsigned accept(const char* login, const char* password);

#endif /* DATABASE_H_ */
