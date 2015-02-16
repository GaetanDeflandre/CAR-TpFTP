#include <stdio.h>
#include <stdlib.h>

#include "database.h"

/* TESTE DE DATABASE */
void test_all_database(){
    init_database();
}

/* FIN TESTE DATABASE */

int main(int argc, char *argv[]){
    
    if(argc != 1){
	fprintf(stderr, "Erreur: mauvais utilisation de %s", argv[0]);
	exit(EXIT_FAILURE);
    }

    test_all_database();

    exit(EXIT_SUCCESS);
}
