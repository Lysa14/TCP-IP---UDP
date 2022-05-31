#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>


#define PORT 8080

int tcpMode()
{
    struct sockaddr_in serv_address;
    
    
    int opt = 1;
    int cl_socket;
    struct sockaddr_in client_address;
    int addrlen_serv = sizeof(serv_address);
    int addrlen_client = sizeof(client_address);
    char buffer[2048];
    int sockfd = 0;
    pid_t childpid;

    //créer une socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Couldn't create socket\n");
        exit(EXIT_FAILURE);
    }
    printf("server socket is created \n");

    //pour réutilliser l'adresse
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))<0){
        fprintf(stderr, "failed to set SO_REUSEADDR\n");
        return 1;
    }
    

    // définir le port et l'IP. 
    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = inet_addr("127.0.0.1"); //htonl(INADDR_ANY);
    serv_address.sin_port = htons(PORT);


    //créer le lien entre la socket et l'adresse du serveur
    if(bind(sockfd, (struct sockaddr *)&serv_address, addrlen_serv)<0)
    {
        fprintf(stderr, "bind failed\n");
        exit(EXIT_FAILURE);
    }
    printf("bind to port \n");

    //mettre la socket en attente de connexion
    if(listen(sockfd, 10)==0){
        printf("listening....\n");
    }else{
        fprintf(stderr, "listen failed\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        //
        if ((cl_socket = accept(sockfd, (struct sockaddr *)&client_address, (socklen_t *)&addrlen_client)) < 0)
        {
            fprintf(stderr, "accept failed\n");
            exit(EXIT_FAILURE);
        }
        printf("connection accepted from %s:%d\n",inet_ntoa(client_address.sin_addr),ntohs(client_address.sin_port));

        if((childpid = fork())==0){
            close(sockfd);
            while(1){
                //recevoir le message du client
                if(recv(cl_socket, buffer, sizeof(buffer), 0)<0){
                    fprintf(stderr, "recv failed\n");
                    exit(EXIT_FAILURE);
                }
                //si le messange envoyé par le client est "exit" alors deconnecter du client 
                if(strcmp(buffer, "exit\n")==0){
                    printf("disconnected from %s:%d\n",inet_ntoa(client_address.sin_addr),ntohs(client_address.sin_port));
                    break;
                }else{
                    printf("client %d: %s\n",ntohs(client_address.sin_port),buffer);
                    //répondre au client 
                    if(send(cl_socket, buffer, strlen(buffer), 0)<0){
                        fprintf(stderr, "send failed\n");
                        exit(EXIT_FAILURE);
                    }
                    //clean buufer
                    bzero(buffer,sizeof(buffer));
                }
            }
        }
    }
    
    close(cl_socket);
    return 0;
}

 int udpMode()
{
   
    /**
     * initialisations des variables 
     * 
     * */

    int sockfd;
    char buffer[2048];
    struct sockaddr_in serv_addr;

    /**
     *Creation de la socketpour intéragir avec les clients
     *sockfd doit étre >0 sinon erreur de creation de soket
     */ 
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr)); //remplissage de la zone mémoire 
    
     /**
     * remplissage des informations du serveur
     * htons :  renvoie la valeur dans l'ordre des octets du réseau TCP/IP. (16bits) / htonl (32bits)
     * IPv4
     * */

    serv_addr.sin_family = AF_INET; // IPv4
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    

    // Liez le socket avec l'adresse du serveur avec bind
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    socklen_t  recv_len , slen = sizeof(serv_addr) ;
    
     /**Boocle while :
	 * recoit de plusieurs messages
     * repend aux clients
	 * */

      while(1)
    {
        printf("Waiting for data...\n");
        fflush(stdout); // pour vider le flux provoqué par stdout

        //recoit les informations
        if ((recv_len = (recvfrom(sockfd, buffer, 2048 , 0, (struct sockaddr *) &serv_addr, &slen)) == -1))
        {
             perror("recvfrom()");
             exit(1);
        }

        printf("Received packet from %s:%d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
        printf("Data: %s\n" , buffer);

        //repend aux clients avec les mémes données
        if (sendto(sockfd, buffer, recv_len, 0, (struct sockaddr *) &serv_addr, slen) == -1)
        {
             perror("sendto()");
             exit(1);
        }
    }

    close(sockfd);

    return 0;
}


int main(int argc, char *argv[]) {

	if (!strcmp(argv[1], "TCP")) {
		printf("salut\n");
		tcpMode(argc, argv);
	} else if (!strcmp(argv[1], "UDP")) {
		udpMode(argc, argv);
	}

	return 0;
}
