#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int main(int argc, char ** argv)
{
	int listenfd, clientfd;
	struct sockaddr_in serv_addr;
	
	char buf[256];
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	memset(&serv_addr, 0, sizeof(struct sockaddr));
	memset(buf, 0, 256);
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(2121);
	
	bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	
	listen(listenfd, 10);
	
	clientfd = accept(listenfd, NULL, NULL);
	snprintf(buf, sizeof(buf), "coucou\n");
	write(clientfd, buf, strlen(buf));
	close(clientfd);
	
	close(listenfd);
}
