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
	data_addr.sin_port = data_addr.sin_port;
	dc->dc_addr = data_addr;
	dc->dc_transfer_t = DT_ACTIVE;
	
	return dc;
}

int open_data_connection(struct s_data_connection * dc)
{
	struct sockaddr_in serv_addr;
	int sock_opt_reuse_addr = 1;
	
	if (dc->dc_socket != -1)
	{
		fprintf(stderr, "Erreur open_data_connection: Connexion deja ouverte.\n");
		return -1;
	}
	
	if (dc->dc_transfer_t == DT_ACTIVE)
	{
		dc->dc_socket = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(dc->dc_socket, SOL_SOCKET, SO_REUSEADDR, &sock_opt_reuse_addr, sizeof(int));
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
	dc->dc_addr.sin_port = htons(port);
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
	char *cr, *str_start;
	ssize_t ret, nb_read_chars=0;
	
	memset(buf, 0, BUF_SIZE + 1);
	
	if (!is_data_connection_opened(dc))
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
	
	while ((ret = read(dc->dc_socket, buf, BUF_SIZE)) != 0)
	{
		if (ret < 0)
		{
			perror("Erreur read_file (read): ");
			unlink(pathname);
			close(fd);
			return -3;
		}
		
		str_start = buf;
		
		while((cr = strchr(str_start, '\r')) != NULL)
		{
			*cr = '\0';
			if (strlen(str_start) > 0)
			{
				if (write(fd, str_start, strlen(str_start)) < 0)
				{
					perror("Erreur read_file (write): ");
					return -4;
				}
			}
			
			str_start = cr + 1;
		}
		
		if (strlen(str_start) > 0)
		{
			if (write(fd, str_start, strlen(str_start)) < 0)
			{
				perror("Erreur read_file (write): ");
				return -4;
			}
		}
		
		nb_read_chars += ret;
	}
		
		////////////////////////
		//~ nl_found = 0;
		//~ 
		//~ if (ret < 0)
		//~ {
			//~ perror("Erreur read_file (read): ");
			//~ unlink(pathname);
			//~ close(fd);
			//~ return -3;
		//~ }
		//~ 
		//~ nl_start = buf;
		//~ while((crlf = strstr(nl_start, "\r\n")) != NULL && nl_start - buf < BUF_SIZE + 1)
		//~ {
			//~ nl_found = 1;
			//~ *crlf = '\n';
			//~ *(crlf + 1) = '\0';
			//~ 
			//~ if (write(fd, nl_start, strlen(nl_start)) < 0)
			//~ {
				//~ perror("Erreur read_file (write): ");
				//~ return -4;
			//~ }
			//~ 
			//~ nl_start += strlen(nl_start) + 1;
		//~ }
		//~ 
		//~ if (!nl_found)
		//~ {
			//~ if (write(fd, buf, strlen(buf)) < 0)
			//~ {
				//~ perror("Erreur read_file (write): ");
				//~ return -4;
			//~ }
		//~ }
		//~ 
		//~ nb_read_chars += ret;
	//~ }
	
	close(fd);
	return nb_read_chars;
}

ssize_t send_file(char * pathname, struct s_data_connection * dc)
{
	char buf[BUF_SIZE + 1];
	char *lf, *str_start, *with_cr_str = NULL;
	int fd;
	size_t new_str_size=0;
	ssize_t ret_write, ret_read, nb_written_chars=0;
	
	memset(buf, 0, BUF_SIZE + 1);
	
	if (!is_data_connection_opened(dc))
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
	
	while ((ret_read = read(fd, buf, BUF_SIZE)) != 0)
	{		
		if (ret_read < 0)
		{
			perror("Erreur read_file (read): ");
			close(fd);
			return -3;
		}
		
		str_start = buf;
		while(str_start < buf + ret_read && (lf = strchr(str_start, '\n')) != NULL)
		{
			*lf = '\0';
			if (with_cr_str == NULL)
			{
				// *2 pour ne pas avoir à réallouer trop souvent.
				with_cr_str = (char*) calloc(BUF_SIZE * 2, sizeof(char));
				new_str_size = BUF_SIZE * 2 * sizeof(char);
				if (with_cr_str == NULL)
				{
					perror("Erreur send_file (calloc)");
					close(fd);
					return -5;
				}
				
			}
			else if (new_str_size - strlen(with_cr_str) < lf - str_start + 3) // 1 pour \r, 1 pour \n et 1 pour \0
			{
				with_cr_str = (char *) realloc(with_cr_str, new_str_size * 2);
				
				if (with_cr_str == NULL)
				{
					perror("Erreur send_file (realloc)");
					close(fd);
					return -5;
				}
				
				memset(with_cr_str + new_str_size, 0, new_str_size);
				new_str_size *= 2;
			}
			
			if(strcat(with_cr_str, str_start) == NULL)
			{
				perror("Erreur strcpy");
				close(fd);
				free(with_cr_str);
				return -5;
			}
			strcat(with_cr_str, "\r\n");
			
			str_start = lf + 1;
		}
		
		if (with_cr_str != NULL)
		{
			printf("str : %s\n", with_cr_str);
			if ((ret_write = write(dc->dc_socket, with_cr_str, strlen(with_cr_str))) < 0)
			{
				perror("Erreur send_file (write)");
				close(fd);
				free(with_cr_str);
				return -4;
			}
			
			/* Ecriture du bout de la chaine qui ne contient pas de \n */
			if ((ret_write = write(dc->dc_socket, str_start, (buf + ret_read) - str_start)) < 0)
			{
				perror("Erreur send_file (write)");
				close(fd);
				free(with_cr_str);
				return -4;
			}
			
			new_str_size = 0;
			free(with_cr_str);
			with_cr_str = NULL;
		}
		else
		{
			if ((ret_write = write(dc->dc_socket, buf, strlen(buf))) < 0)
			{
				perror("Erreur send_file (write)");
				close(fd);
				return -4;
			}
			memset(buf, 0, BUF_SIZE + 1);
		}
		
		nb_written_chars += ret_write;
	}
	
	/////////////////////
	//~ while ((ret = read(fd, buf, BUF_SIZE+1)) != 0)
	//~ {
		//~ if (ret < 0)
		//~ {
			//~ perror("Erreur send_file (read): ");
			//~ return -3;
		//~ }
		//~ 
		//~ if (write(dc->dc_socket, buf, BUF_SIZE+1) < 0)
		//~ {
			//~ perror("Erreur send_file (write): ");
			//~ return -4;
		//~ }
		//~ 
		//~ nb_read_chars += ret;
	//~ }
	
	close(fd);
	return nb_written_chars;
}
