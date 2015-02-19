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
	 |-[+] doc
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
 - etc : fichier de configuration de la documentation et la base de
   données

### Documentation

Pour ouvrir la documentation un script `./opendoc.sh` exsiste, il
ouvre index.html du répertoire doc.

Les graphes des dépandances, graphes des inclusion, et schémas des
structure sont disponible dans la documentation.


## Gestion des erreurs

La valeur de retour des fonction est systématiquement vérifier,
variable errno affiché.


## Codes intéressants

### Serveur multi processus

Les processus sont obtenus avec la fonction `fork()`, le père attends
de nouveaux clients. Les processus zombies sont géré.

    if(init_sigaction(&action) == -1){
	fprintf(stderr, "Erreur: lors de l'initialisation du handler wait_child.\n");
	return -1;
    }

    if(sigaction(SIGCHLD, &action, NULL) == -1){
	perror("Erreur signaction: ");
	return -1;
    }
    
    while(1){

		clientfd = accept(listenfd, (struct sockaddr *) &cli_addr, &sock_len);
		if(clientfd == -1){
			perror("Erreur accept: ");
			return -1;
		}

		printf("Nouveau client\n");
		   
		cpid = fork();
		switch (cpid)
		{
			case -1: /* Erreur */
			perror("Erreur fork: ");
			return -1;

			case 0: /* Fils */
			handle_client(cli_addr, clientfd);
			close(clientfd);
		
			return 0;
			break;
		
			default: /* Père */
				close(clientfd);
		}
    }
    
    close(clientfd);	
    close(listenfd);


### Gestionnaire du client

    /* Boucle de traitement de requetes. Une requete par tour. */
	while(1)
	    {
		
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

### Commande générique

La fonction des commandes est changé dans une variable afin de
généraliser les commandes

    /**
     * @struct s_cmd
     * Structure de la commande d'un client.
     */
    struct s_cmd;

    /**
     * Type de la fonction handler commande.
     */
    typedef void (cmd_handler)(struct s_cmd *);


    struct s_cmd
    {
        /** Fonction à appeler */
        cmd_handler * cmd_h;
        /** Arguments de la commande */
        char * cmd_args_field;
        /** Type de la commande */
        enum cmd_type cmd_t;
        /** Client à l'origine de la commande */
        struct s_client * cmd_client;
    };


### Commande FILE

La commande liste renvoie duffèrente réponses au client, selon divers
paramètre.

    void process_list(struct s_cmd * cmd)
    {
        ...

        client = cmd->cmd_client;

        if(client == NULL){
            fprintf(stderr, "Erreur: client faut null.\n");
            status = -1;
        }

        /* ARGUMENT */
        if(strcpy(path, client->cli_current_path) == NULL){
            perror("Erreur strcpy: ");
            status = -1;
        }
        if(cmd->cmd_args_field != NULL){
            if(strcat(path, "/") == NULL){
                perror("Erreur strcat: ");
                status = -1;
            }
            if(strcat(path, cmd->cmd_args_field) == NULL){
                perror("Erreur strcat: ");
                status = -1;
            }
        }

        /* OPEN DIR */
        if(status>=0){
            pDir = opendir(path);
            if (pDir == NULL) {
                perror("Erreur opendir: ");
                status = -2;
            }
        }

        /* GET FOLDER CHILDREN  */
        if(status>=0){
            if((pDirent = readdir(pDir)) != NULL){

                if(strncpy(bufData, pDirent->d_name, BUF_SIZE) == NULL){
                    perror("Erreur strncpy: ");
                    status = -1;
                }

                if(strcat(bufData, "  ")== NULL){
                    perror("Erreur strcat: ");
                    status = -1;
                }

                while ((pDirent = readdir(pDir)) != NULL) {
                    /* Recherche dossiers et fichiers */
                    ...
                }

                strcat(bufData, "\r\n");

            } else {
                bufData[0] = '\0';
            }
        }

        if(status>=0){

            /* SEND LIST*/
            c_data_connect = client->cli_data_connection;
            if(c_data_connect == NULL){
                fprintf(stderr, "Erreur: champ cli_data_connection null.\n");
                status = -1;
            }

            if(c_data_connect->dc_transfer_t == DT_ACTIVE){

                if (!is_data_connection_opened(c_data_connect)){

                    if(open_data_connection(c_data_connect) == -1){
                        fprintf(stderr, "Erreur: Ouverture connection data.\n");
                        status = -3;
                    }

                    /* File status okay; about to open data connection. */
                    snprintf(buf, BUF_SIZE, "150 File status okay; about to open data connection.\r\n");

                    if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
                        perror("Erreur write: ");
                        return;
                    }
                } else {
                /* Data connection already open; transfer starting. */
                snprintf(buf, BUF_SIZE, "125 Data connection already open; transfer starting.\r\n");

                if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
                    perror("Erreur write: ");
                    return;
                }
            }

            write_data(bufData, c_data_connect);
            printf("--- buf envoyé: %s\n", bufData);

            if(close_data_connection(c_data_connect) == -1){
                fprintf(stderr, "Erreur: Fermeture connection data.\n");
                status = -1;
            }

            /* Closing data connection. */
            snprintf(buf, BUF_SIZE, "226 Closing data connection.\r\n");

            if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
                perror("Erreur write: ");
                return;
            }
            return;

            } else {
                /* Command not implemented for that parameter. */
                snprintf(buf, BUF_SIZE, "504 Command not implemented for that parameter.\r\n");

                if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
                    perror("Erreur write: ");
                    status = -1;
                }
            }
        }

        if(status == -1){
            /* Requested action aborted: local error in processing. */
            snprintf(buf, BUF_SIZE, "451 Requested action aborted: local error in processing.\r\n");

            if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
                perror("Erreur write: ");
                return;
            }
        } else if(status == -2){
            /* Syntax error in parameters or arguments. */
            snprintf(buf, BUF_SIZE, "501 Syntax error in parameters or arguments.\r\n");

            if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
                perror("Erreur write: ");
                return;
            }
        } else if(status == -3){
            /* Can't open data connection. */
            snprintf(buf, BUF_SIZE, "425 Can\'t open data connection.\r\n");

            if(write(cmd->cmd_client->cli_sock, buf, strlen(buf)) == -1){
                perror("Erreur write: ");
                return;
            }
        }

        return;
    }
