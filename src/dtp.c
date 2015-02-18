#include "dtp.h"

#include <stdio.h>
#include <stdlib.h>

struct s_data_connection * new_data_connection(struct sockaddr_in data_addr)
{
	struct s_data_connection * dc;
	
	dc = malloc(sizeof(struct s_data_connection));
	if (dc == NULL)
	{
		fprintf(stderr, "Erreur new_data_connection: erreur malloc\n");
		return NULL;
	}
	
	dc->dc_socket = -1;
	dc->dc_addr = data_addr;
	dc->dc_transfer_t = DT_ACTIVE;
	
	return dc;
}

int open_data_connection(struct s_data_connection * dc)
{
	fprintf(stderr, "NYI !\n");
	return -1;
}

int close_data_connection(struct s_data_connection * dc)
{
	fprintf(stderr, "NYI !\n");
	return -1;
}

void set_port(struct s_data_connection * dc, unsigned short port)
{
	dc->dc_addr.sin_port = port;
}

void set_transfer_t_active(struct s_data_connection * dc)
{
	dc->dc_transfer_t = DT_ACTIVE;
}

void set_transfer_t_passive(struct s_data_connection * dc)
{
	dc->dc_transfer_t = DT_PASSIVE;
}

int is_data_connection_opened(struct s_data_connection * dc)
{
	fprintf(stderr, "NYI !\n");
	return -1;
}

ssize_t write_message(char * message, struct s_data_connection * dc)
{
	fprintf(stderr, "NYI !\n");
	return -1;
}

ssize_t read_file(char * pathname, struct s_data_connection * dc)
{
	fprintf(stderr, "NYI !\n");
	return -1;
}

ssize_t send_file(char * pathname, struct s_data_connection * dc)
{
	fprintf(stderr, "NYI !\n");
	return -1;
}
