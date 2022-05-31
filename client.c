#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int tcpMode(int argc, char *argv[])
{
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	char buffer[2048];
	int sockfd;
	// créer une socket 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "Couldn't create socket\n");
		exit(EXIT_FAILURE);
	}
	//créer une structure d'adresse réseau
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		fprintf(stderr, "Invalid address/ Address not supported \n");
		exit(EXIT_FAILURE);
	}
	//envoyer une demande de connexion au serveur
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "Connection Failed \n");
		exit(EXIT_FAILURE);
	}
	printf("Connected\n");

    while(1){
        printf("client: \t");
	    fgets(buffer, sizeof(buffer),stdin);
		//envoyer un message au serveur
	    if(send(sockfd, buffer, strlen(buffer), 0)<0)
		{
			fprintf(stderr, "send failed\n");
			exit(EXIT_FAILURE);
		}
		
        if(strcmp(buffer, "exit\n")==0){
			close(sockfd);
			printf("disconneced from server\n");
            break;
        }
		//recevoir une réponse de la part du serveur
	    if (recv(sockfd, buffer, 2048, 0) < 0)
	    {
			fprintf(stderr, "recv failed\n");
			exit(EXIT_FAILURE);
		}
	    else
	    {
			printf("server: \t%s\n", buffer);
	    }
    }
	return 0;
    
}

int udpMode(int argc, char *argv[]) {

/**
 * initialisations des variables 
 * 
 * */
	struct sockaddr_in serv_addr;
    char buffer[2048];
	int sockfd ;
    

	/**
     *Creation de la socket ntéragir avec le server
     *sockfd doit étre >0 sinon erreur de creation de soket
     */ 

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

    memset((char *) &serv_addr, 0, sizeof(serv_addr)); //remplissage de la zone mémoire 
    
	 /**
     * remplissage des informations du client
     * htons :  renvoie la valeur dans l'ordre des octets du réseau TCP/IP. (16bits)
     * */
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	
    
    char message[2048];
    socklen_t slen = sizeof(serv_addr);
    
	/** Boocle while : 
	 * envoit de un ou de plusieurs messages au server
	 * 
	 * */
	 
	 while(1)
    {
        printf("Enter message : ");
        fgets(message,sizeof(message), stdin);

        //envoyé le / les message au server
        if (sendto(sockfd, message, strlen(message) , 0 , (struct sockaddr *) &serv_addr,slen) == -1)
        {
             perror("sendto()");
             exit(1);
        }

        memset(buffer,'\0', 2048); 
        
        if (recvfrom(sockfd, buffer, 2048 , 0, (struct sockaddr *) &serv_addr, &slen) == -1)
        {
             perror("recvfrom()");
             exit(1);
        }

        puts(buffer);
    }

	close(sockfd);
	return 0;
}

 

int main(int argc, char *argv[])
{

	if (!strcmp(argv[1], "TCP"))
	{
		printf("salut\n");
		tcpMode(argc, argv);
	}else if (!strcmp(argv[1], "UDP")) {
		udpMode(argc, argv);
	}
	

	return 0;
}