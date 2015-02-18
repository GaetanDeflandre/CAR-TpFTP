#ifndef DTP_H_
#define DTP_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/** Type de transfert de données : passif ou non. */
enum data_transfer_type {DT_ACTIVE, DT_PASSIVE};

struct s_data_connection
{
	/** Socket de transert de données. Vaut -1 si pas de connexion
     *  ouverte */
    int dc_socket;
    /** Adresse du client pour le transfert de données (avec le port) */
    struct sockaddr_in dc_addr;
    /** Type de transfert de données */
    enum data_transfer_type dc_transfer_t;
};

struct s_data_connection * new_data_connection(struct sockaddr_in data_addr);
int open_data_connection(struct s_data_connection * dc);
int close_data_connection(struct s_data_connection * dc);
void set_port(struct s_data_connection * dc, unsigned short port);
void set_transfer_t_active(struct s_data_connection * dc);
void set_transfer_t_passive(struct s_data_connection * dc);
int is_data_connection_opened(struct s_data_connection * dc);
ssize_t write_message(char * message, struct s_data_connection * dc);
ssize_t read_file(char * pathname, struct s_data_connection * dc);
ssize_t send_file(char * pathname, struct s_data_connection * dc);


#endif /* DTP_H_ */
