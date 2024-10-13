#include "OBEP.h"
#include <mysql.h>





int clients[NB_MAX_CLIENTS];
int nbClients = 0;

MYSQL * connexion;

int estPresent(int socket);
void ajoute(int socket);
void retire(int socket);

pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER; // initialiser un mutex mais de manière statique pas dynamique(=> pthread_mutext_init())


bool OBEP(char* requete, char* reponse,int socket)
{
	char *ptr = strtok(requete,"#");
	MYSQL_RES *results;
	MYSQL_ROW tuple;

	connexion = mysql_init(NULL);

	if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(OBEP) Erreur de connexion à la base de données...\n");
        exit(1);  
    }

    // CAS DE LOGOUT
	if(strcmp(ptr,"LOGOUT") == 0)
	{
		printf("%\t[THREAD %p] LOGOUT\n",pthread_self());
		sprintf(reponse,"LOGOUT#ok");
		retire(socket);
	}


	// en cas de LOGIN
	if(strcmp(ptr,"LOGIN")== 0)
	{
		char user[50],password[50];
		strcpy(user,strtok(NULL,"#"));
		strcpy(password,strtok(NULL,"#"));
		printf("%\t[THREAD %p] LOGIN de %s\n",pthread_self(),user);
		if(estPresent(socket) >= 0)
		{
			sprintf(reponse,"LOGIN#ko#Client déjà loggé !");
		}
		else
		{
			if(OBEP_Login(user,password))
			{
				sprintf(reponse,"LOGIN#ok");
				ajoute(socket);
			}
			else
			{
				sprintf(reponse,"LOGIN#ko#Mauvais identifiants !");
			}
		}
	}

	// AUTRE CAS
	else
	{
		if(estPresent(socket) == -1)
		{
			sprintf(reponse,"LOGIN#ko#Client déjà loggé !");
		}
		else
		{
			//Recherche les livres
			if(strcmp(ptr, "GET_BOOKS") == 0)
			{
				if(mysql_query(connexion, "SELECT b1.id, b1.title, CONCAT(a1.first_name, ' '; a1.last_name) as author_name, s1.name as subject_name, b1.isbn, b1.page_count, b1.publish_year, b1.price, b1.stock_quantity FROM books b1 INNER JOIN authors a1 on b1.author_id = a1.id INNER JOIN subjects s1 on b1.subject_id = s1.id;"))
				{
					sprintf(reponse,"GET_BOOKS#ko#Erreur requete : %s", mysql_error(connexion));
				}
				else 
				{
					results = mysql_store_result(connexion);
					if (results == NULL)
					{
						sprintf(reponse, "GET_BOOKS#ko#Erreur lors de la récupération des livres");
					}
					else
					{
	            		sprintf(reponse, "GET_BOOKS#ok#");
	            		while((tuple = mysql_fetch_row(results))) {
	                		sprintf(reponse + strlen(reponse), "#%s#%s#%s#%s#%s#%s#%s#%s#%s;", tuple[0], tuple[1], tuple[2], tuple[3], tuple[4], tuple[5], tuple[6], tuple[7], tuple[8]);
	            		}
            		}
            		mysql_free_result(results);
            	}
			}


			//Recherche les auteurs
			if(strcmp(ptr, "GET_AUTHORS") == 0)
			{
				if(mysql_query(connexion, "SELECT id, last_name, first_name FROM authors"))
				{
					sprintf(reponse,"GET_AUTHORS#ko#Erreur requete : %s", mysql_error(connexion));
				}
				else 
				{
					results = mysql_store_result(connexion);
					if (results == NULL)
					{
						sprintf(reponse, "GET_AUTHORS#ko#Erreur lors de la récupération des auteurs");
					}
					else
					{
	            		sprintf(reponse, "GET_AUTHORS#ok#");
	            		while((tuple = mysql_fetch_row(results))) {
	                		sprintf(reponse + strlen(reponse), "#%s#%s#%s;", tuple[0], tuple[1], tuple[2]);
	            		}
            		}
            		mysql_free_result(results);
            	}


			}
			//Recherche les sujets
			if(strcmp(ptr, "GET_SUBJECTS") == 0)
			{
				if(mysql_query(connexion, "SELECT id, name FROM subjects"))
				{
					sprintf(reponse,"GET_SUBJECTS#ko#Erreur requete : %s", mysql_error(connexion));
				}
				else 
				{
					results = mysql_store_result(connexion);
					if (results == NULL)
					{
						sprintf(reponse, "GET_SUBJECTS#ko#Erreur lors de la récupération des sujets");
					}
					else
					{
	            		sprintf(reponse, "GET_SUBJECTS#ok#");
	            		while((tuple = mysql_fetch_row(results))) {
	                		sprintf(reponse + strlen(reponse), "#%s#%s;", tuple[0], tuple[1]);
	            		}
            		}
            		mysql_free_result(results);
            	}

			}




			//Ajoute un auteur si non retourne -1
			if(strcmp(ptr, "ADD_AUTHOR") == 0)
			{
				char requete[500];
				char firstName[100], lastName[100], birth_date[20];

				strcpy(lastName, strtok(NULL, "#"));
    			strcpy(firstName, strtok(NULL, "#"));
    			strcpy(birth_date, strtok(NULL, "#"));


    			sprintf(requete, "INSERT INTO authors(first_name, last_name, birth_date) VALUES ('%s', '%s', '%s')", 
    						firstName, lastName, birth_date);

    			if(mysql_query(connexion, requete))
    			{
    				sprintf(reponse,"ADD_AUTHOR#ko#Erreur requete : %s", mysql_error(connexion));
    			}
    			else
    			{
    				sprintf(reponse, "ADD_AUTHOR#ok#", mysql_insert_id(connexion));
    			}


			}
			//Ajoute un auteur si non retourne -1
			if(strcmp(ptr, "ADD_SUBJECT") == 0)
			{
				char requete[500];
				char name[100];

				strcpy(name, strtok(NULL, "#"));


    			sprintf(requete, "INSERT INTO authors(name) VALUES ('%s')", 
    						name);

    			if(mysql_query(connexion, requete))
    			{
    				sprintf(reponse,"ADD_SUBJECT#ko#Erreur requete : %s", mysql_error(connexion));
    			}
    			else
    			{
    				sprintf(reponse, "ADD_SUBJECT#ok#", mysql_insert_id(connexion));
    			}

			}
			//Ajoute un auteur si non retourne -1
			if(strcmp(ptr, "ADD_BOOK") == 0)
			{
				char requete[500];
				char title[100], isbn[20], authorLastName[20], authorFirstName[20], subject[20];
				int pageCount, publishYear, stock;
				float price;

				strcpy(title, strtok(NULL, "#"));
    			strcpy(isbn, strtok(NULL, "#"));
    			pageCount = atoi(strtok(NULL, "#"));
    			price = atof(strtok(NULL, "#"));
    			publishYear = atoi(strtok(NULL, "#"));
    			stock = atoi(strtok(NULL, "#"));
				strcpy(authorFirstName, strtok(NULL, "#"));
    			strcpy(authorLastName, strtok(NULL, "#"));
				strcpy(subject, strtok(NULL, "#"));


    			
    			
    			
    			


    			sprintf(requete, "INSERT INTO books(author_id, subject_id, title, isbn, page_count, stock_quantity, price, publish_year) VALUES ((SELECT id FROM authors WHERE first_name = '%s' AND last_name = '%s'), (SELECT id FROM subjects WHERE name = '%s'), '%s', '%s', '%d', '%d', '%f', '%d')", 
    						authorFirstName, authorLastName, subject, title, isbn, pageCount, stock, price, publishYear);

    			if(mysql_query(connexion, requete))
    			{
    				sprintf(reponse,"ADD_BOOK#ko#Erreur requete : %s", mysql_error(connexion));
    			}
    			else
    			{
    				sprintf(reponse, "ADD_BOOK#ok#", mysql_insert_id(connexion));
    			}

			}



		}

	}

	return true;
}
bool OBEP_Login(const char* user,const char* password)
{
	if(strcmp(user,"adri")==0 && strcmp(password,"123")==0) return true;
	// requete sql ici normalement pour accèes bd
	return false;
}
int OBEP_Operation(char op,int a,int b)
{
	// acces bd requete sql
}
// EN CAS DE FIN PRÉMATURÉ 
void OBEP_Close()
{
	for(int i =0;i<nbClients;i++)
	{
		close(clients[i]);
	}
	pthread_mutex_unlock(&mutexClients);
}

// permet de vérifier la connexion d'un client
int estPresent(int socket)
{
	int indice = -1;
	pthread_mutex_lock(&mutexClients);
	for(int i=0 ; i<nbClients ; i++)
	if (clients[i] == socket) { indice = i; break; }
	pthread_mutex_unlock(&mutexClients);
	return indice;
}

// ajoute le client vient de se connecter
void ajoute(int socket)
{
	pthread_mutex_lock(&mutexClients);
	clients[nbClients] = socket;
	nbClients++;
	pthread_mutex_unlock(&mutexClients);
}

// retire un client qui vient de déconnecter
void retire(int socket)
{
	int pos = estPresent(socket);
	if (pos == -1) return;
	pthread_mutex_lock(&mutexClients);
	for (int i=pos ; i<=nbClients-2 ; i++)
	clients[i] = clients[i+1];
	nbClients--;
	pthread_mutex_unlock(&mutexClients);
}
