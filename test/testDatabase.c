#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "database.h"


void test_all_database();
void test_get_use_info_user_founded();
void test_get_use_info_user_not_founded();

void test_all_database(){
    test_get_use_info_user_founded();
    test_get_use_info_user_not_founded();
}

void test_get_use_info_user_founded(){

    /* wordking case*/

    char* mdp;
    char* path;
    int status;
    
    status = get_user_info("test", &mdp, &path);

    assert(status == 1);
    assert(strcmp(mdp, "testmdp") == 0);
    assert(strcmp(path, "files/test") == 0);

    printf("OK: get_user_info, cas utilisateur trouvé.\n");
}

void test_get_use_info_user_not_founded(){


    char* mdp;
    char* path;
    int status;
    
    status = get_user_info("_", &mdp, &path);

    assert(status == 0);

    printf("OK: get_user_info, cas utilisateur non trouvé.\n");
}


int main(int argc, char *argv[]){
    
    if(argc != 1){
	fprintf(stderr, "Erreur: mauvais utilisation de %s", argv[0]);
	exit(EXIT_FAILURE);
    }

    test_all_database();

    printf("\nOK: fonctions de database.\n");

    exit(EXIT_SUCCESS);
}
