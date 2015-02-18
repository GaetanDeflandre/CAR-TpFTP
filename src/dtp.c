#include "dtp.h"
#include "communication.h"
#include "servFTP.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
	data_addr.sin_port = ntohs(data_addr.sin_port);
	dc->dc_addr = data_addr;
	dc->dc_transfer_t = DT_ACTIVE;
	
	return dc;
}

int open_data_connection(struct s_data_connection * dc)
{
	struct sockaddr_in serv_addr;
	
	if (dc->dc_socket != -1)
	{
		fprintf(stderr, "Erreur open_data_connection: Connexion deja ouverte.\n");
		return -1;
	}
	
	if (dc->dc_transfer_t == DT_ACTIVE)
	{
		dc->dc_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (dc->dc_socket < 0)
		{
			perror("Erreur open_data_connection (socket): ");
			return -1;
		}
		
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port = htons(DEFAULT_DATA_PORT);
		
		if ((bind(dc->dc_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0)
		{
			perror("Erreur open_data_connection (bind): ");
			return -1;
		}
		
		if ((connect(dc->dc_socket, (struct sockaddr *) &(dc->dc_addr), sizeof(dc->dc_addr))) < 0)
		{
			perror("Erreur open_data_connection (connect): ");
			return -1;
		}
	}
	else if (dc->dc_transfer_t == DT_PASSIVE)
	{
		fprintf(stderr, "Passive mode NYI !\n");
		return -1;
	}
	else
	{
		fprintf(stderr, "Erreur open_data_connection: type de transfert inconnu ou invalide.\n");
		return -1;
	}
	
	return 1;
}

int close_data_connection(struct s_data_connection * dc)
{
	int ret;
	
	ret = close(dc->dc_socket);
	dc->dc_socket = -1;
	
	if (ret == -1)
		perror("Erreur close_data_connection (close): ");
		
	return ret;
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
	return dc != NULL && dc->dc_socket >= 0;
}

ssize_t write_data(char * message, struct s_data_connection * dc)
{
	char buf[BUF_SIZE+1];
	
	if (dc->dc_socket < 0)
	{
		fprintf(stderr, "Erreur open_data_connection: Connexion inexistante.\n");
		return -1;
	}
	
	strncpy(buf, message, BUF_SIZE+1);
	
	return write_socket(dc->dc_socket, buf);
}

ssize_t read_file(char * pathname, struct s_data_connection * dc)
{
	char buf[BUF_SIZE + 1];
	int fd;
	ssize_t ret, nb_read_chars=0;
	
	if (dc->dc_socket < 0)
	{
		fprintf(stderr, "Erreur read_file: Connexion données non-ouverte.\n");
		return -1;
	}
	
	fd = creat(pathname, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd < 0)
	{
		perror("Erreur read_file (creat): ");
		return -2;
	}
	
	while ((ret = read(dc->dc_socket, buf, BUF_SIZE+1)) != 0)
	{
		if (ret < 0)
		{
			perror("Erreur read_file (read): ");
			return -3;
		}
		
		if (write(fd, buf, BUF_SIZE + 1) < 0)
		{
			perror("Erreur read_file (write): ");
			return -4;
		}
		
		nb_read_chars += ret;
	}
	
	close(fd);
	return nb_read_chars;
}

ssize_t send_file(char * pathname, struct s_data_connection * dc)
{
	char buf[BUF_SIZE + 1];
	int fd;
	ssize_t ret, nb_read_chars=0;
	
	if (dc->dc_socket < 0)
	{
		fprintf(stderr, "Erreur send_file: Connexion données non-ouverte.\n");
		return -1;
	}
	
	fd = open(pathname, O_RDONLY);
	if (fd < 0)
	{
		perror("Erreur send_file (open): ");
		return -2;
	}
	
	while ((ret = read(fd, buf, BUF_SIZE+1)) != 0)
	{
		if (ret < 0)
		{
			perror("Erreur send_file (read): ");
			return -3;
		}
		
		if (write(dc->dc_socket, buf, BUF_SIZE + 1) < 0)
		{
			perror("Erreur send_file (write): ");
			return -4;
		}
		
		nb_read_chars += ret;
	}
	
	close(fd);
	return nb_read_chars;
}
