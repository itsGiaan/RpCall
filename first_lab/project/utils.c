#include "utils.h"

//HOSTED METHODS.

//Somma di due interi.
int add(int num1, int num2) { return num1 + num2; }

//Differenza tra due float.
float difference(float num1, float num2) { return num1 - num2; }

//Differenza tra due interi.
int diff(int num1, int num2) { return num1 - num2; }

//Somma di due float.
float addition(float num1, float num2) { return num1 + num2; }

//Divisione tra due interi.
int divide(int num1, int num2){ return num1 / num2; }

//Divisione tra due float.
float division(float num1, float num2){ return num1 / num2; }

//Prodotto tra due interi.
int prod(int num1, int num2){ return num1 * num2; }

//Prodotto tra due float.
float product(float num1, float num2){ return num1 * num2; }


//UTILITY FUNCTIONS.

//Per leggere count bytes da un descrittore.
ssize_t FullRead(int fd, void *buf, size_t count)
{
    size_t nleft; //left bytes to read
    size_t nread; //bytes already readed
    nleft = count;

    while( nleft > 0 )
    {
        if ((nread = read(fd, buf, nleft)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                exit(nread);
            }
        }
        else if ( nread == 0 )
        {
            return -1;
        }
        nleft -= nread;
        buf += nread;
    }
    buf = 0;
    return(nleft);
}

//Per scrivere count bytes su un descrittore.
ssize_t FullWrite(int fd, const void *buf, size_t count)
{
    size_t nleft, nwritten;
    nleft = count;

    while( nleft > 0 )
    {
        nwritten = write(fd, buf, nleft);
        if ((nwritten) < 0)
        {
            if ( errno == EINTR )
            {
                continue;
            }
            else
            {
                exit(nwritten);
            }
        }
        nleft -= nwritten;
        buf += nwritten;
    }
    return(nleft);
}


//Per generare una stringa random di lunghezza lenght.
char *random_string(size_t length) 
{ 
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; 
    char *randomString;

    if (length) {
    randomString = malloc(length +1); 

    if (randomString) {
        int l = (int) (sizeof(charset) -1);
        int key;  
        for (int n = 0;n < length;n++) {        
            key = rand() % l;   
            randomString[n] = charset[key];
        }

        randomString[length] = '\0';
    }
}
return randomString;
}

//Per stampare a video un oggetto di tipo net_node (le informazioni di un peer).
void printStruct(struct net_node object)
{
    printf("\n*************************************************************************************************************************************************\n");
    printf("\nIp: %s\t", object.ip);
    printf("\tPort: %hd", object.port);
    printf("\tName: %s", object.name);
    printf("\tDescription: %s", object.description);
    printf("\tParam1: %s", object.param1);
    printf("\tParam2: %s\n", object.param2);
    printf("\n*************************************************************************************************************************************************\n");
}

//Controllo lineare in lista per verificare la presenza di un peer.
int isInList(struct LinkedList *list, char *ip)
{
    struct net_node tmp;
    for(int i = 0; i < list->length; i++)
    {
        memset(&tmp, 0, sizeof(struct net_node));
        tmp = (*(struct net_node*)list->retrieve(list, i));
        if(strcmp(tmp.ip, ip) == 0)
        {
            return 1;
        }
    }
    return 0;
}


short isKnown(struct LinkedList *list, char *ip)
{
    struct net_node tmp;
    in_addr_t client, retrieved;
    client = inet_addr(ip);

    for(int i = 0; i < list->length; i++)
    {
        memset(&tmp, 0, sizeof(struct net_node));
        tmp = (*(struct net_node*)list->retrieve(list, i));
        retrieved = inet_addr(tmp.ip);
        if(client == retrieved)
        {
            return 1;
        }
    }
    return 0;
}

//Per stampare a video la LinkedList di oggetti net_node.
void printList(struct LinkedList *list)
{
    struct net_node object;
    for(int i = 0; i < list->length; i++)
    {
        object = (*(struct net_node*) list->retrieve(list, i));
        printf("\nNode: %d\n", i);
        printStruct(object);
    }
}

//Per convertire la stringa che rappresenta l'output della procedura richiesta al corretto tipo.
void setResult(struct net_node tmp, char *result)
{
    if(strcmp(tmp.param1, "int") == 0)
    {
        int retVal = atoi(result);
        printf("\nRisultato: %d from %s\n", retVal, tmp.ip);
    }
    else if(strcmp(tmp.param1, "float") == 0)
    {
        float retVal = atof(result);
        printf("\nResult: %f from %s\n", retVal, tmp.ip);
    }
    else
    {
        printf("\nWrong result from %s\n", tmp.ip);
    }
}


void asciiART()
{
    printf("\n");
    printf("'########::'########:::'######:::::'###::::'##:::::::'##:::::::\n");
    printf(" ##.... ##: ##.... ##:'##... ##:::'## ##::: ##::::::: ##:::::::\n");
    printf(" ##:::: ##: ##:::: ##: ##:::..:::'##:. ##:: ##::::::: ##:::::::\n");
    printf(" ########:: ########:: ##:::::::'##:::. ##: ##::::::: ##:::::::\n");
    printf(" ##.. ##::: ##.....::: ##::::::: #########: ##::::::: ##:::::::\n");
    printf(" ##::. ##:: ##:::::::: ##::: ##: ##.... ##: ##::::::: ##:::::::\n");
    printf(" ##:::. ##: ##::::::::. ######:: ##:::: ##: ########: ########:\n");
    printf("..:::::..::..::::::::::......:::..:::::..::........::........::\n");
    printf("\n");
}


static unsigned int i2a(char *dest, unsigned int x)
{
    register unsigned int tmp = x;
    register unsigned int len = 0;
    if(x >= 100) { *dest++=tmp/100+'0'; tmp = tmp%100; ++len; }
    if(x >= 10) {*dest++=tmp/10+'0'; tmp = tmp%10; ++len; }
    *dest++=tmp+'0';
    return len +1;
}

char *inet_ntoa_r(struct in_addr in, char* buf)
{
    unsigned int len;
    unsigned char *ip = (unsigned char*)&in;
    len = i2a(buf, ip[0]); buf[len]= '.'; ++len;
    len += i2a(buf + len, ip[1]); buf[len]= '.'; ++len;
    len += i2a(buf + len, ip[2]); buf[len]= '.'; ++len;
    len += i2a(buf + len, ip[3]); buf[len] = 0;
    return buf;
}