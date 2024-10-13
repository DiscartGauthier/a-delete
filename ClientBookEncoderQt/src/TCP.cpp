#include "TCP.h"


int ServerSocket(int bearing)
{
	// création de la socket
	int s=0;
	char const *p = to_string(bearing).c_str();

	printf("pid = %d\n",getpid());

	// création de la socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("Erreur de socket()");
		exit(1);
	}
	printf("socket creee = %d\n",s);


	// Construction de l'adresse
	struct addrinfo hints;
	struct addrinfo *results;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family=AF_INET; // => protocole fondé sur IPv4
	hints.ai_socktype=SOCK_STREAM; //=> car mode connecté (TCP)
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; // pour une connexion passive
	if(getaddrinfo(NULL,p,&hints,&results) != 0) // mettre NULL car on crée un SERVEUR ICI ip => 0.0.0.0 et le port souahité
	{
		printf("erreur getaddrinfo");
		exit(1);
	}
	
	// Affichage du contenu de l'adresse obtenue
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];

	getnameinfo(results->ai_addr,results->ai_addrlen,
	host,NI_MAXHOST,port,NI_MAXSERV,NI_NUMERICSERV | NI_NUMERICHOST);
	printf("Mon Adresse IP: %s -- Mon Port: %s\n",host,port);


	// Liaison de la socket à l'adresse
	if (bind(s,results->ai_addr,results->ai_addrlen) < 0)
	{
	 	perror("Erreur de bind()");
	 	exit(1);
	}
	freeaddrinfo(results);
	printf("bind() reussi !\n");

	return s; // renvoi la socket
}

int ClientSocket(char* ipServeur, int portServeur)
{
	// création de la socket
	int s=0;

	printf("pid = %d\n",getpid());

	// création de la socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("Erreur de socket()");
		exit(1);
	}
	printf("socket creee = %d\n",s);	

	// Construction de l'adresse
	struct addrinfo hints;
	struct addrinfo *results;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family=AF_INET; // => protocole fondé sur IPv4
	hints.ai_socktype=SOCK_STREAM; //=> car mode connecté (TCP)
	hints.ai_flags = AI_NUMERICSERV;
	if(getaddrinfo(ipServeur,to_string(portServeur).c_str(),&hints,&results) != 0) 
	{
		printf("erreur getaddrinfo");
		//exit(1);
	}
	// Demande de connexion
 	if (connect(s,results->ai_addr,results->ai_addrlen) == -1)
 	{
 		printf("Erreur de connect()");
 		return-1;
 	}
 	else
 	{
 		printf("connect() reussi !\n");
 	}
 	
	return s;
}

int Accept(int sEcoute, char* ipClient)
{
	if(listen(sEcoute,SOMAXCONN)==-1)
	{
		perror("Erreur de listen()");
		exit(1);
	}
	printf("listen() reussi !\n");

	// Attente d'une connexion
	int sService;
	if ((sService = accept(sEcoute,NULL,NULL)) == -1)
	{
	 	perror("Erreur de accept()");
	 	exit(1);
	}
	
	printf("accept() reussi !\n");
	printf("socket de service = %d\n",sService);

	// Recuperation d'information sur le client connecte
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	struct sockaddr_in adrClient;
	socklen_t adrClientLen = sizeof(struct sockaddr_in); // nécessaire
	getpeername(sService,(struct sockaddr*)&adrClient,&adrClientLen);
	getnameinfo((struct sockaddr*)&adrClient,adrClientLen,
	host,NI_MAXHOST,
	port,NI_MAXSERV,
	NI_NUMERICSERV | NI_NUMERICHOST);
	printf("Client connecte --> Adresse IP: %s -- Port: %s\n",host,port);	
	strcpy(ipClient,host);
	return sService;
}

int Send(int sSocket,char* data,int taille)
{
	int nb=0;
	// premier écriture pour savoir la taille de la requête.
	if((nb = write(sSocket,&taille,sizeof(taille)))==-1)
	{
		perror("Erreur de write pour la taille");
		close(sSocket);
	}
	printf("(Taille) nbEcrit = %d Ecrit : --%d--\n",nb,taille);

	// deuxième écriture qui envoit la requête
	if((nb = write(sSocket,data,taille)) == -1)
	{
		perror("Erreur de write()");
		close(sSocket);
	}
	else
	{
		printf("nbEcrit = %d Ecrit : ---%s---\n",nb,data);
	}
	
	return nb;
}
int Receive(int sSocket,char* data)
{
	int nb =0;
	int taille=0;

	// première lecture pour savoir la taille de la requête
	if((nb=read(sSocket,&taille,sizeof(taille)))==-1)
	{
		perror("Erreur de read pour la taille");
		close(sSocket);
	}
	printf("(Taille) nbLu = %d Lu : --%d--\n",nb,taille);

	if((nb=read(sSocket,data,taille))==-1)
	{
		perror("Erreur de read()");
		close(sSocket);
	}
	printf("nbLu = %d Lu : ---%s---\n",nb,data);

	return nb;
}
