#include <stdio.h>
#include <stdlib.h>

#include "database.h"


void test_all_database(){
    login("toto", NULL, NULL);
}


int main(int argc, char *argv[]){
    
    if(argc != 1){
	fprintf(stderr, "Erreur: mauvais utilisation de %s", argv[0]);
	exit(EXIT_FAILURE);
    }

    test_all_database();

    exit(EXIT_SUCCESS);
}
