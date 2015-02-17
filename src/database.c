#include <string.h>
#include "database.h"

static xmlNodePtr _root;

/* PRIVATE */

/*
 * Initialise la base de données, change le XML.
 * Retourne 0 si bon fonctionnement, -1 sinon.
 */
int init_database(const char* filename);

/* 
 * Retourne la racine du DOM xlm de la base de donnée.
 */
xmlNodePtr get_root();

/*
 * Parcours récursive des noeuds du DOM, tant que l'utilisateur 
 * n'est pas retrouvé.
 */
int walker(const xmlNodePtr node, const char* name, char** password, char** path);

int walker(const xmlNodePtr node, const char* name, char** password, char** path);


int init_database(const char* filename){

    xmlDocPtr doc;
    xmlNodePtr first;

    /* Ouvrir xml */
    doc = xmlParseFile(filename);
    if(doc == NULL){
	fprintf(stderr, "Erreur: echec ouverture document xml.\n");
	return -1;
    }

    /* Récupérer la racine */
    first = xmlDocGetRootElement(doc);
    if(first == NULL){
	fprintf(stderr, "Erreur: document XML vierge.\n");
	return -1;
    }

    _root = first;
    return 0;
}

xmlNodePtr get_root(){
    if(_root == NULL){
	if(init_database(DATABASE_FILE) == -1){
	    return NULL;
	}
    }
    return _root;
}

int get_user_info(const char* name, char** password, char** path){
    
    xmlNodePtr root = get_root();

    if(root == NULL){
	fprintf(stderr, "Erreur: echec chargement XML ou XML viège.\n");
	return -1;
    }

    return walker(root, name, password, path);
}

int walker(const xmlNodePtr node, const char* name, char** password, char** path){
    
    xmlNodePtr n;
    xmlChar* xml_name = NULL;
    xmlChar* xml_password = NULL;

    for(n = node; n!=NULL; n=n->next){

	if(n->type == XML_ELEMENT_NODE && strcmp((char*)(n->name), "login") == 0){
	    xml_name = xmlNodeGetContent(n);
	    if(strcmp((char*)xml_name, name) == 0){
		printf("Utilisateur existant.\n");
		printf("%s: %s\n", n->name, xml_name);

		n=n->next;
		n=n->next;
		if(n == NULL && n->type == XML_ELEMENT_NODE){
		    fprintf(stderr, "Erreur: fichier XML mal formaté.\n");
		    xmlFree(xml_name);
		    return -1; 
		}
		
		if(strcmp((char*)n->name, "password") == 0){
		    xml_password = xmlNodeGetContent(n);

		    *password = malloc(sizeof(char) * PASSWORD_MAXLEN);
		    if(*password != NULL){
			strncpy(*password, (char*)xml_password, PASSWORD_MAXLEN);
		    }
		    if(*password == NULL){
			fprintf(stderr, "Erreur: lors de la copy du mot de passe.\n");
			xmlFree(xml_name);
			xmlFree(xml_password);
			return -1;
		    }

		    printf("%s: %s\n", n->name, xml_password);
		    xmlFree(xml_name);
		    xmlFree(xml_password);
		    return 1;
		} else {
		    fprintf(stderr, "Erreur: pas de mot de passe après login.\n");
		    xmlFree(xml_name);
		    return -1;
		}
	    }
	}


	if ((n->type == XML_ELEMENT_NODE) && (n->children != NULL)) {
	    return walker(n->children, name, password, path);
        } 
    }
    
    return 0;
}

