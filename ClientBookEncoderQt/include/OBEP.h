#ifndef OBEP_H
#define OBEP_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <mysql.h>
#include "TCP.h"


#define NB_MAX_CLIENTS 100


bool OBEP(char* requete, char* reponse,int socket);
bool OBEP_Login(const char* user,const char* password);
int OBEP_Operation(char op,int a,int b);
void OBEP_Close();

#endif