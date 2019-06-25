#include "socket.h"

int creer_serveur ( int port ){

	int socket_serveur ;
	struct sockaddr_in saddr ;
	int optval = 1;
	saddr.sin_family = AF_INET ; /* Socket ipv4 */
	saddr.sin_port = htons(port); /* Port d ecoute */
	saddr.sin_addr.s_addr = INADDR_ANY ; /* ecoute sur toutes les interfaces */


	socket_serveur = socket ( AF_INET , SOCK_STREAM , 0);
	if ( socket_serveur == -1)
	{
		/* traitement de l ’ erreur */
		perror ( "Creation socket_serveur" );
	}
	/* Utilisation de la socket serveur */
	if(setsockopt(socket_serveur,SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(int)) == -1)
		perror("Impossible d'utiliser SO_REUSEADDR");


	if ( bind ( socket_serveur , ( struct sockaddr *)& saddr , sizeof ( saddr )) == -1)
	{
		perror ( "Parametrage socker_serveur" );
		/* traitement de l ’ erreur */
	}

	if(listen(socket_serveur, 10) == -1)
	{
		perror("Ecoute socket_serveur");
	}

	return socket_serveur;
}
