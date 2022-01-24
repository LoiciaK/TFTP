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

int requete(char* filename, char* mode, int sfd, struct addrinfo *res){
	
	int lenRRQ = 3+strlen(filename)+strlen(mode)+1;	// taille de RRQ d'après RFC 1350
	char* RRQ=(char *)malloc(lenRRQ*sizeof(char));
	RRQ[0]=0;
	RRQ[1]=1; //former 01 pour OPCode qui se trouve dans RFC 1350
	
	strcpy(RRQ+2, filename); //string filename stocké à partir du bit2 et de longueur filename
	RRQ[2+strlen(filename)]=0; // 1 byte 
	
	strcpy(RRQ+3+strlen(filename), mode);
	RRQ[3+strlen(filename)+strlen(mode)]=0;
	
	ssize_t nbr_sent = sendto(sfd, RRQ, lenRRQ, 0, res->ai_addr, res->ai_addrlen); // sfd est le socket créé, RRQ la requête 
	if(nbr_sent == -1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}
	printf("Le nombre d'octets émis est : %ld\n", nbr_sent); // 9+le nombre de caractères du nom de fichier envoyé
	
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
	hints.ai_family = AF_INET; // famille d'adresses : IPV4
	hints.ai_socktype = SOCK_DGRAM; // type de socket associé à l'UDP
	hints.ai_protocol = IPPROTO_UDP; // protocole UDP
	s = getaddrinfo(host,"1069",&hints,&res); // choix du port 1069 sur lequel le serveur TFTP est disponible 
	// getaddrinfo retourne une ou plusieurs structures de données, chacune contient une adresse IPv4
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
    

    requete(file, "octet", sfd, res); //on choisit le mode de transmission en octets



    int lenDATA = 516; //512 octets est la taille max en mode octets + 4 octets pour coder 03 et Block# du paquet DATA
    char* DAT=(char *)malloc(lenDATA*sizeof(char)); // allocation de mémoire pour DAT de taille maximale

    ssize_t nbr_recv = recvfrom(sfd, DAT, lenDATA, 0, res->ai_addr, &(res->ai_addrlen));//flags 0
    // on stocke dans DAT les données reçues 
    if(nbr_recv == -1){
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}
	printf("Le nombre d'octets reçus est : %ld\n", nbr_recv); 
	
	char* ACK = "";
	ACK[0]=0;
	ACK[1]=4; // La valeur de l'OP code est 4 codé sur 2 octets : 04
	int lenACK = 4; // la taille du paquet ACK est de 4 octets (RFC1350)
	int f = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC); //on crée le fichier s'il n'est pas déjà créé en mode écriture et le fichier est écrasé s'il existe déjà
	ACK[2]= DAT[2];
	ACK[3]=DAT[3]; // le numéro du paquet recu codé sur 2 octets 
	write(f, DAT+4,lenDATA); // on écrit dans le fichier f les caractères à partir de DAT[4] qui correspond au début de la chaîne de caractères 
	// les 4 caractères précédents correspondent à l'Opcode et au Block# (le numéro du paquet)
	ssize_t nbr_ack = sendto(sfd, ACK, lenACK, 0, res->ai_addr, res->ai_addrlen); //l'acquittement est envoyé au serveur 
	if(nbr_ack == -1){
		perror("sendto");
		exit(EXIT_FAILURE);
	} 
    
}


