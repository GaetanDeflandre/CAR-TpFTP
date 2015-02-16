#include "database.h"

#include <libxml/tree.h>
#include <libxml/parser.h>


/*static list_user_t database;*/

int init_database(char* filename){

    xmlDocPtr doc;
    /*xmlNodePtr root;*/

    /* */
    doc = xmlParseFile(filename);
    if(doc == NULL){
	fprintf(stderr, "Erreur: echec ouverture document xml\n");
	return -1;
    }


    printf("coucou\n");
    return -1;
}

/*int load_database(list_user_t* database){
    return -1
    }*/

