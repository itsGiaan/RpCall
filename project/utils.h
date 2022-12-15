#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/types.h>
#include "LinkedList.h"
#include "server.h"

//DATA TYPES

//Gli attributi che verranno inviati al peer che offre la procedura richiesta.
//Invieremo delle stringhe al fine di avere la procedura di inserimento dei parametri uguale per tutti.
//Successivamente il peer che riceve l'oggetto attributes provvederà alla conversione nel giusto tipo.
struct attributes
{
    char param1[10];
    char param2[10];
};

//Le informazioni dei peer e delle procedure offerte che verranno salvate dai partecipanti alla rete.
struct net_node
{
    char ip[INET_ADDRSTRLEN];
    short port;
    char name[20];
    char description[255];
    char param1[10];
    char param2[10];
};

//Gli argomenti che verranno passati al nuovo thread creato per gestire la richiesta.
struct ServerLoopArgument
{
    int client;
    struct Server *server;
    struct LinkedList *known_hosts;
};


//HOSTED METHODS PROTOTYPES.

//Somma di due interi.
int add(int num1, int num2);

//Differenza tra due float.
float difference(float num1, float num2);

//Differenza tra due interi.
int diff(int num1, int num2);

//Somma di due float.
float addition(float num1, float num2);

//Divisione tra due interi.
int divide(int num1, int num2);

//Divisione tra due float.
float division(float num1, float num2);

//Prodotto tra due interi.
int prod(int num1, int num2);

//Prodotto tra due float.
float product(float num1, float num2);



//UTILITY FUNCTIONS.

//Per leggere count bytes da un descrittore.
ssize_t FullRead(int fd, void *buf, size_t count);

//Per scrivere count bytes su un descrittore.
ssize_t FullWrite(int fd, const void *buf, size_t count);

//Per generare una stringa random di lunghezza lenght.
char *random_string(size_t length);

//Per stampare a video un oggetto di tipo net_node (le informazioni di un peer).
void printStruct(struct net_node object);

//Controllo lineare in lista per verificare la presenza di un peer.
int isInList(struct LinkedList *list, char *ip);

short isKnown(struct LinkedList *list, char *ip);

//Per stampare a video la LinkedList di oggetti net_node.
void printList(struct LinkedList *list);

//Per convertire la stringa che rappresenta l'output della procedura richiesta al corretto tipo.
void setResult(struct net_node tmp, char *result);

void asciiART();

static unsigned int i2a(char *dest, unsigned int x);

char *inet_ntoa_r(struct in_addr in, char* buf);

#endif