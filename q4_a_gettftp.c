#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


 /* Construction d’une requête en lecture (RRQ) correctement formée, et envoie au serveur */
int gettftp(char* filename, char* mode, int sfd, struct addrinfo *res){
	
	int lenRRQ = 3+strlen(filename)+strlen(mode)+1;	 // taille du buffer réservé (3 octets + longueur du fichier en argument + longueur de mode + 1 octet) (voir RFC 1350)
	char* RRQ=(char *)malloc(lenRRQ*sizeof(char));  // on alloue une place en mémoire pour la requête
	RRQ[0]=0;
	RRQ[1]=1; //former 01 pour OPCode qui se trouve dans RFC 1350
	
	strcpy(RRQ+2, filename); //string filename stocké à partir du bit2 et de longueur filename
	
	RRQ[2+strlen(filename)]=0; // 1 byte (doc RFC 1350)
	
	strcpy(RRQ+3+strlen(filename), mode); // on ajoute le mode 
	
	RRQ[3+strlen(filename)+strlen(mode)]=0; // un dernier octet (doc RFC 1350)
	
	ssize_t nbr_sent = sendto(sfd, RRQ, lenRRQ, 0, res->ai_addr, res->ai_addrlen); // on envoie au serveur à l'adresse 127.0.0.1
	
	if(nbr_sent == -1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}
	
	// on s'attend à avoir émis 9 octets + la longueur de la chaîne de caractère rentrée en argument (1 caractère = 1 octet)
	printf("Le nombre de bits émis est : %ld\n", nbr_sent);
	
	return 0;
}





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
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	s = getaddrinfo(host,"1069",&hints,&res);
	
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

	/*Réservation d'un socket de connexion vers le serveur*/
    
    
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); // descripteur référençant la socket créée
        if (sfd < 0){    // erreur
			perror("socket error");
			exit(EXIT_FAILURE);
            continue;
		}
		printf("Le socket reservé est : %d\n", sfd);
         
    }
    
  /* Construction d’une requête en lecture (RRQ) correctement formée, et envoie au serveur */
  
    gettftp(file, "octet", sfd, res); // on choisit le mode de transmission en octets
    
}


