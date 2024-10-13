#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <time.h>
#include <string.h>

typedef struct {
  int  id;
  char last_name[20];
  char first_name[20];
  char birth_date[20];
} AUTHOR;

typedef struct {
  int  id;
  char name[20];
} SUBJECT;

typedef struct {
  int   id;
  int   author_id;    // clé étrangère
  int   subject_id;   // clé étrangère
  char  title[100];
  char  isbn[20];
  int   page_count;
  int   stock_quantity;
  float price;
  int   publish_year;
} BOOK;

typedef struct {
  int id;
  char login[50];
  char password[50];
} EMPLOYEES;

typedef struct {
  int id;
  char name[20];
  char firstname[20];
  int clientNr;
  char adresse[100];
  char email[100];
  char telNumber[20];
  // on peut encore rajouter mais bon pas besoin de plus je pense
} CLIENTS;

typedef struct {
  int id;
  int clientId;
  char date[20];
  float amount;
  float payed;
} CADDIES;

typedef struct {
  int id;
  int caddyId;
  int bookId; // clé étrangère
  int quantity; // clé étrangère
} CADDY_ITEMS;


AUTHOR authors[] = {
  {-1,"Werber","Bernard","1961-09-18"},  // id = 1
  {-1,"Brown","Dan","1964-06-22"},       // id = 2
  {-1,"Herbert","Frank","1920-10-08"},   // id = 3
  {-1,"Cixin","Liu","1963-06-23"},       // id = 4
  {-1,"Delannoy","Claude","1946-01-06"}, // id = 5
  {-1,"Le Cun","Yan","1960-07-08"},      // id = 6
  {-1,"Verne","Jules","1828-02-08"},     // id = 7
  {-1,"Harris","Thomas","1940-09-022"}   // id = 8
};
int nbAuthors = 8;

SUBJECT subjects[] = {
  {-1,"Science-Fiction"},                // id = 1
  {-1,"Thriller"},                       // id = 2
  {-1,"Informatique"}                    // id = 3
};
int nbSubjects = 3;

BOOK books[] = {
  {-1,1,1,"Les Thanatonautes","978-2253139225",505,3,9.70f,1999},
  {-1,1,1,"Les Fourmis","978-2253063339",312,10,9.99f,1998},
  {-1,1,1,"La boite de Pandore","978-2253934332",576,1,9.70f,2020},
  {-1,2,2,"Da Vinci Code","978-2709624930",574,6,22.90f,2004},
  {-1,2,2,"Anges et demons","978-2253093008",720,2,10.40f,2015},
  {-1,3,1,"Dune","978-2266320481",928,13,11.95f,2021},
  {-1,4,1,"Le probleme a trois corps","978-2330181055",512,0,11.00f,2024},
  {-1,5,3,"Programmer en C++ moderne: De C++11 a C++20","978-2212678956",848,1,38.00f,2019},
  {-1,5,3,"Programmer en Java","978-2212118896",916,8,71.18f,2016},
  {-1,6,3,"Quand la machine apprend","978-2415006563",394,2,11.50f,2023},
  {-1,7,1,"Vingt mille lieues sous les mers","978-2253006329",606,11,6.70f,2001},
  {-1,3,1,"Les heretiques de Dune","978-2221264577",598,2,24.90f,2023},
  {-1,8,2,"Le silence des agneaux","978-2266208949",377,17,7.70f,2015}
};
int nbBooks = 13;

void finish_with_error(MYSQL *con);

int main(int argc,char *argv[])
{
  // Connection a MySql
  printf("Connection a la BD...\n");
  MYSQL* connexion = mysql_init(NULL);
  mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0);

  // Supprimer la table books si elle existe déjà
  if (mysql_query(connexion, "DROP TABLE IF EXISTS books;")) {
    finish_with_error(connexion);
  }

  // Supprimer la table authors si elle existe déjà
  if (mysql_query(connexion, "DROP TABLE IF EXISTS authors;")) {
    finish_with_error(connexion);
  }

  // Supprimer la table subjects si elle existe déjà
  if (mysql_query(connexion, "DROP TABLE IF EXISTS subjects;")) {
    finish_with_error(connexion);
  }

  // Supprimer la table employees si elle existe déjà
  if (mysql_query(connexion, "DROP TABLE IF EXISTS employees;")) {
    finish_with_error(connexion);
  }

  // Supprimer la table clients si elle existe déjà
  if (mysql_query(connexion, "DROP TABLE IF EXISTS clients;")) {
    finish_with_error(connexion);
  }

  // Supprimer la table caddies si elle existe déjà
  if (mysql_query(connexion, "DROP TABLE IF EXISTS caddies;")) {
    finish_with_error(connexion);
  }

  // Supprimer la table caddy_items si elle existe déjà
  if (mysql_query(connexion, "DROP TABLE IF EXISTS caddy_items;")) {
    finish_with_error(connexion);
  }

  // Creation de la table authors
  printf("Creation de la table authors...\n");
  if (mysql_query(connexion,"CREATE TABLE authors ("
                      "id INT(4) AUTO_INCREMENT PRIMARY KEY, "
                      "last_name VARCHAR(20), "
                      "first_name VARCHAR(20), "
                      "birth_date DATE);")) {
    finish_with_error(connexion);
  }

  // Creation de la table subjects
  printf("Creation de la table subjects...\n");
  if (mysql_query(connexion,"CREATE TABLE subjects ("
                      "id INT(4) AUTO_INCREMENT PRIMARY KEY, "
                      "name VARCHAR(20));")) {
    finish_with_error(connexion);
  }

  // Créer la table books
  printf("Creation de la table books...\n");
  if (mysql_query(connexion, "CREATE TABLE books ("
                       "id INT(4) AUTO_INCREMENT PRIMARY KEY, "
                       "author_id INT, "
                       "subject_id INT, "
                       "title VARCHAR(100), "
                       "isbn VARCHAR(20), "
                       "page_count INT, "
                       "stock_quantity INT, "
                       "price DECIMAL(10, 2), "
                       "publish_year INT, "
                       "FOREIGN KEY (author_id) REFERENCES authors(id), "
                       "FOREIGN KEY (subject_id) REFERENCES subjects(id));")) {
    finish_with_error(connexion);
  }
/*
  // Créer la table employees
  printf("Creation de la table employees...\n");
  if (mysql_query(connexion, "CREATE TABLE employees ("
                       "id INT(4) AUTO_INCREMENT PRIMARY KEY, "
                       "login VARCHAR(50), "
                       "password VARCHAR(50));")) {
    finish_with_error(connexion);
  }

  // Créer la table clients
  printf("Creation de la table clients...\n");
  if (mysql_query(connexion, "CREATE TABLE clients ("
                       "id INT(4) AUTO_INCREMENT PRIMARY KEY, "
                       "name VARCHAR(20), "
                       "firstname VARCHAR(20), "
                       "clientNr INT, "
                       "adresse VARCHAR(100), "
                       "email VARCHAR(100), "
                       "telNumber VARCHAR(20));")){
    finish_with_error(connexion);
  }

    // Créer la table caddies
  printf("Creation de la table caddies...\n");
  if (mysql_query(connexion, "CREATE TABLE caddies ("
                       "id INT(4) AUTO_INCREMENT PRIMARY KEY, "
                       "clientId INT, "
                       "date VARCHAR(), "
                       "amount INT, "
                       "payed INT, "
                       "FOREIGN KEY (clientId) REFERENCES clients(id));")) {
    finish_with_error(connexion);
  }

    // Créer la table caddy_items
  printf("Creation de la table caddy_items...\n");
  if (mysql_query(connexion, "CREATE TABLE caddy_items ("
                       "id INT(4) AUTO_INCREMENT PRIMARY KEY, "
                       "caddyId INT, "
                       "bookId INT, "
                       "quantity INT, "
                       "FOREIGN KEY (caddyId) REFERENCES caddies(id), "
                       "FOREIGN KEY (bookId) REFERENCES books(id));")) {
    finish_with_error(connexion);
  }*/

  // Ajout de tuples dans la table authors
  printf("Ajout de %d auteurs la table authors...\n",nbAuthors);
  char request[256];
  for (int i=0 ; i<nbAuthors ; i++) {
    sprintf(request,"insert into authors values (NULL,'%s','%s','%s');",
      authors[i].last_name,authors[i].first_name,authors[i].birth_date);
    if (mysql_query(connexion,request) ) {
      finish_with_error(connexion);   
    }
  }


  // Ajout de tuples dans la table subjects
  printf("Ajout de %d sujets la table subjects...\n",nbSubjects);
  for (int i=0 ; i<nbSubjects ; i++) {
    sprintf(request,"insert into subjects values (NULL,'%s');",subjects[i].name);
    if (mysql_query(connexion,request)) {
      finish_with_error(connexion);   
    }
  }


  // Ajout de tuples dans la table books
  printf("Ajout de %d livres la table books...\n",nbBooks);
  for (int i=0 ; i<nbBooks ; i++) {
    sprintf(request,"INSERT INTO books (author_id, subject_id, title, isbn, page_count, stock_quantity, price, publish_year) VALUES (%d, %d, '%s', '%s', %d, %d, %f, %d);", 
      books[i].author_id,books[i].subject_id,books[i].title,books[i].isbn,books[i].page_count,books[i].stock_quantity,books[i].price,books[i].publish_year);
    if (mysql_query(connexion,request)) {
      finish_with_error(connexion);   
    }
  }

  // Deconnection de la BD
  mysql_close(connexion);
  exit(0);
}

void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}