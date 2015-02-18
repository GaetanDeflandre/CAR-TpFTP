Serveur FTP
===========


## Auteurs

 - Samuel GRANDSIR
 - Gaëtan DEFLANDRE


## Introduction

Serveur FTP, multi processus, écrit en C. Le projet utilise les
protocoles TCP/IP et FTP pour le transfert de fichiers.


## Dépendance

Pour compiler le project la bibliothèque *libxml2* est nécessaire.

    $ apt_get install libxml2


## Architecture

Le projet est agencé de la manière suivante:

[-] .
 |-[-] doc
 |-[-] etc
 |  |-  database.xml
 |  `-  doxygen.ini
 |-[-] files
 |  |-[+] test
 |  `-[+] toto
 |-[-] include
 |  |-  clientHandler.h
 |  |-  command.h
 |  |-  communication.h
 |  |-  database.h
 |  |-  dtp.h
 |  `-  servFTP.h
 |-[-] src
 |  |-  clientHandler.c
 |  |-  command.c
 |  |-  communication.c
 |  |-  database.c
 |  |-  dtp.c
 |  `-  servFTP.c
 |-[-] test
 |  |-  testDatabase.c
 |  `-  testServFTP.c
 |-  Makefile
 |-  README.md
 `-  opendoc.sh

Le contenu détailler des diffèrents répertoire est décrit ci-dessous:

 - src : les fichiers sources (.c)
 - include : les fichier d'en-tête (.h)
 - test : répertoire de teste
 - doc : répertoire de documentation
 - etc : fichier de configuration de la documentation et la base de données


## Codes intéressants

### Handler client

    /* Boucle de traitement de requetes. Une requete par tour. */
	while(1)
	    {
		printf("u= %s\np= %s\n", client.cli_username, client.cli_current_path);
		
		/* Lecture requete */
		req_size = read_client_request(client.cli_sock, &request);
		printf("Size of request: %d\nRequest: %s\n", req_size, request);
		
		/* Traitement de la requete */
		destroy_cmd(cmd);
		cmd = init_cmd(request, &client);
		if (cmd != NULL)
		    {
			printf("Type : %d\n", cmd->cmd_t);
			/* Si la requete necessite un login */
			if (needing_login_cmd(cmd))
			    {
				if (!client.cli_logged_in)
				    {
					snprintf(buf, BUF_SIZE, "530 Not logged in.\r\n");
					write_socket(client.cli_sock, buf);
					
					continue;
				    }
			    }
			else if (cmd->cmd_t == CMD_PASS)
			    {
				if (!waitingForPassword)
				    {
					snprintf(buf, BUF_SIZE, "503 Bad sequence of commands.\r\n");
					write_socket(client.cli_sock, buf);
					
					continue;
				    }
			    } else if (cmd->cmd_t == CMD_USER) // (Re)commencement du login.
			    {
				memset(client.cli_current_path, 0, PATHNAME_MAXLEN);
				free(client.cli_username);
				client.cli_username = NULL;
				client.cli_logged_in = 0;
			    }
			
			/* Execution de la requete */
			exec_cmd(cmd);
			
			if (waitingForPassword)
			    {
				if (!client.cli_logged_in)
				    {
					memset(client.cli_current_path, 0, PATHNAME_MAXLEN);
					free(client.cli_username);
					client.cli_username = NULL;
				    }
				waitingForPassword = 0;
			    }
			
			/* Commande USER avec succes. */
			if (cmd->cmd_t == CMD_USER && client.cli_username != NULL)
			    waitingForPassword = 1;
		    }
		else
		    {
			/* Requete incorrecte. */
			fprintf(stderr, "Erreur cmd: request=%s\n", request);
			snprintf(buf, BUF_SIZE, "500 Syntax error, command unrecognized.\r\n");
			write_socket(client.cli_sock, buf);
			
			if (waitingForPassword)
			    {
				if (!client.cli_logged_in)
				    {
					memset(client.cli_current_path, 0, PATHNAME_MAXLEN);
					free(client.cli_username);
					client.cli_username = NULL;
				    }
				waitingForPassword = 0;
			    }
		    }
		
		/* Connexion terminée. */
		if (cmd != NULL && cmd->cmd_t == CMD_QUIT)
		    return;
	    }


### Commande FILE
