#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "database.h"


void test_all_database();
void test_get_use_info();


void test_all_database(){
    test_get_use_info();
}

void test_get_use_info(){

    /* wordking case*/

    char* mdp;
    char* path;
    int status;
    
    status = get_user_info("test", &mdp, &path);

    assert(status == 1);
    assert(strcmp(mdp, "testmdp") == 0);
    assert(strcmp(path, "files/test") == 0);

    printf("OK: get_user_info");
}


int main(int argc, char *argv[]){
    
    if(argc != 1){
	fprintf(stderr, "Erreur: mauvais utilisation de %s", argv[0]);
	exit(EXIT_FAILURE);
    }

    test_all_database();

    printf("\nOK: all database fonctions\n");

    exit(EXIT_SUCCESS);
}
