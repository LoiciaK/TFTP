#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[]){
	
	/*Récupération des arguments dans la ligne de commande*/ 
	
	char* host;
	char* file;
	if(argc == 3){ // 3 arguments dans la ligne de commande (argv[0]=fichier exécuté, host, file)
		host = argv[1];
		file = argv[2];
		printf("host : %s\n", host);
		printf("file : %s\n", file);
	}
	else{
		perror("pas assez d'arguments");
		exit(EXIT_FAILURE);
	}
	
	/*Récupération de l'adresse du serveur*/
	
	// on va déclarer une structure addrinfo dans laquelle on va récupérer l'adresse IP du serveur
	struct addrinfo hints, *res, *rp;
	char host_return[NI_MAXHOST]; // NI_MAXHOST correspond à une taille de 1025
	int s;
	memset(&hints,0,sizeof(struct addrinfo)); // sizeof donne la taille d'une structure de données
	hints.ai_family = AF_INET; // seulement les adresses de type IPv4
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;
	
	
	s = getaddrinfo(host,NULL,&hints,&res); // renvoie 0 si elle réussit
	if(s==0){
		
		// on va afficher l'adresse Ipv4 avec les différents protocoles associés: UDP, TCP, IP 
		// (ai_family, ai_socktype et ai_protocol changent de valeurs
		// si on avait laissé ai_family à 0, on aurait 6 sorties (3 de plus correspondant à l'adresse Ipv6)
		
		for(rp=res;rp!=0;rp=rp->ai_next){
			printf("ai_family = %d\nai_socktype = %d\nai_protocol = %d\n", rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			getnameinfo(rp->ai_addr,rp->ai_addrlen,host_return,sizeof(host_return),NULL,0,NI_NUMERICHOST | NI_NUMERICSERV);
			printf("L'adresse IP est : %s\n",host_return);
		}
	
	}
	else{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
    }


	
}
