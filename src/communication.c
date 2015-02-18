#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

ssize_t write_socket(int socket, char * buf)
{
	ssize_t nb_bytes_written;
	
	nb_bytes_written = write(socket, buf, strlen(buf));
	if(nb_bytes_written == -1)
	{
		perror("Erreur write: ");
		exit(EXIT_FAILURE);
	}
	
	return nb_bytes_written;
}
