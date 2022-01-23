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
	
	struct addrinfo hints, *res, *rp;
	char host_return[NI_MAXHOST];
	int s, sfd;
	memset(&hints,0,sizeof(struct addrinfo)); //sizeof donne la taille d'une structure de données
	hints.ai_family = AF_INET;
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;
	s = getaddrinfo(host,NULL,&hints,&res);
	if(s==0){
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
