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

int gettftp(char* filename, char* mode, int sfd, struct addrinfo *res){
	
	int lenRRQ = 3+strlen(filename)+strlen(mode)+1;	
	char* RRQ=(char *)malloc(lenRRQ*sizeof(char));
	RRQ[0]=0;
	RRQ[1]=1; //former 01 pour OPCode qui se trouve dans RFC 1350
	
	strcpy(RRQ+2, filename); //string filename stocké à partir du bit2 et de longueur filename
	RRQ[2+strlen(filename)]=0; // 1 byte 
	
	strcpy(RRQ+3+strlen(filename), mode);
	RRQ[3+strlen(filename)+strlen(mode)]=0;
	
	ssize_t nbr_sent = sendto(sfd, RRQ, lenRRQ, 0, res->ai_addr, res->ai_addrlen);
	if(nbr_sent == -1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}
	printf("Le nombre de bits émis est : %ld\n", nbr_sent);
	
	return 0;
}

//receivefrom : reception, sendto : acquittement = dire au 



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
    
    

    
    //sfd = socket(AF_INET, rp->ai_socktype, 0);
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); // descripteur référençant la socket créée
        if (sfd < 0){    // erreur
			perror("socket error");
			exit(EXIT_FAILURE);
            continue;
		}
		//printf("Le socket reservé est : %ls\n", &myaddr.sin_addr.s_addr);
            //break;   // Success
		
        //close(sfd);
    }
    
    struct sockaddr_in myaddr;
    memset(&myaddr,0,sizeof(struct sockaddr_in)); //sizeof donne la taille d'une structure de données
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(1069);
    inet_aton("127.0.0.1", &myaddr.sin_addr);
    
    gettftp(argv[2], "octet", sfd, res); //on choisit le mode de transmission en octets
    
}


