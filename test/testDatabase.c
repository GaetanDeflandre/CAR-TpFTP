#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "database.h"


int test_all_database();
int test_get_use_info();


int test_all_database(){

    if(test_get_use_info() == -1){
	return -1;
    }

    return 0;
}

int test_get_use_info(){

    char* mdp;
    char* path;
    int status;
    
    status = get_user_info("toto", &mdp, &path);

    printf("Status: %d\n", status); 
    printf("%s\n", mdp);
    printf("%s\n", path);
    
    /*assert();*/

    return 0;
}


int main(int argc, char *argv[]){
    
    if(argc != 1){
	fprintf(stderr, "Erreur: mauvais utilisation de %s", argv[0]);
	exit(EXIT_FAILURE);
    }

    if(test_all_database() == -1){
	exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
