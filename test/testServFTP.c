#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]){
    
    if(argc != 1){
	fprintf(stderr, "Erreur: mauvais utilisation de %s", argv[0]);
	exit(EXIT_FAILURE);
    }



    exit(EXIT_SUCCESS);
}
