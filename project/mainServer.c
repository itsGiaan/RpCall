#include "LinkedList.h"
#include "server.h"
#include "utils.h"
#include "wrapper.h"

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
void * server_loop(void *arg);

int main()
{
    struct Server server = server_init(AF_INET, SOCK_STREAM, IPPROTO_TCP, INADDR_ANY, 9930, 20);
    struct LinkedList known_nodes = linked_list_init();
    struct net_node tmp;
    socklen_t slen = (socklen_t)sizeof(server.address);
    pthread_t server_thread;
    FILE *fp;
    fp = fopen("peers.dat", "r+");

    if(fp != NULL)
    {
        while(fread(&tmp, sizeof(struct net_node), 1, fp) > 0) 
        {
            known_nodes.insert(&known_nodes, known_nodes.length, &tmp, sizeof(struct net_node));
        }
        fclose(fp);
    }

    asciiART();
    printf("\nRandezvous server running...\n");
    printf("\nCurrent nodes: %d\n", known_nodes.length);
   
   while(1)
    {
        struct ServerLoopArgument loop_arg;
        loop_arg.client = Accept(server.socket, (struct sockaddr*)&server.address, &slen);
        loop_arg.server = &server;
        loop_arg.known_hosts = &known_nodes;

        pthread_create(&server_thread, NULL, server_loop, &loop_arg);
    }


    close(server.socket);
    linked_list_destroy(&known_nodes);
    return 0;
}


void * server_loop(void *arg)
{
    struct ServerLoopArgument *arguments = arg;
    int len = arguments->known_hosts->length;
    int usage, flag, found;
    FILE *filePointer;
    struct net_node tmp;
    char *client_address = inet_ntoa(arguments->server->address.sin_addr);

    //Verifico la natura del peer.
    FullRead(arguments->client, &usage, sizeof(int));
    usage = ntohl(usage);

    if(usage) //Se si tratta di un semplice utente.
    {
        if(!len)
        {
            usage = -1;
            usage = htonl(usage);
            //Comunico al peer che al momento non vi sono host disponibili.
            FullWrite(arguments->client, &usage, sizeof(int));
            close(arguments->client);
        }
        else
        {
            len = htonl(len);
            //Comunico la lunghezza della lista
            FullWrite(arguments->client, &len, sizeof(int));
            len = ntohl(len);
            for(int i = 0; i < len; i++)
                {
                    memset(&tmp, 0, sizeof(struct net_node));
                    tmp = (*(struct net_node*) arguments->known_hosts->retrieve(arguments->known_hosts, i));
                    printf("\nSending To -> %s: \n", client_address);
                    printStruct(tmp);
                    FullWrite(arguments->client, &tmp, sizeof(struct net_node));
                }
            close(arguments->client);
        }
    }

    if (!usage)//Se si tratta di un nodo.
    {    
        if(len == 0 )
        {
            len = htonl(len);
            //Comunico 0 affinche' il peer sappia che ho bisogno delle sue informazioni.
            FullWrite(arguments->client, &len, sizeof(int));
            //Inserimento in lista
            for(int i = 0; i < 3; i++)
            {
                memset(&tmp, 0, sizeof(struct net_node));
                FullRead(arguments->client, &tmp, sizeof(struct net_node));
                strncpy(tmp.ip, client_address, INET_ADDRSTRLEN);
                pthread_mutex_lock(&file_mutex);
                filePointer = fopen("peers.dat", "a+");
                fwrite(&tmp, sizeof(struct net_node), 1, filePointer);
                fclose(filePointer);
                pthread_mutex_unlock(&file_mutex);
                arguments->known_hosts->insert(arguments->known_hosts, i, &tmp, sizeof(struct net_node));
            }
            printf("\nFirst node added\n");
            printList(arguments->known_hosts);
            close(arguments->client);
        }
        else
        {
            found = isInList(arguments->known_hosts, client_address);
            //Si tratta di un nuovo nodo.
            if(found == 0)
            {   
                printf("\nNode: %s not found\n", client_address);
                len = htonl(len);
                //Comunico la lunghezza della lista.
                FullWrite(arguments->client, &len, sizeof(int));
                len = ntohl(len);
                for(int i = 0; i < len; i++)
                {
                    memset(&tmp, 0, sizeof(struct net_node));
                    tmp = (*(struct net_node*) arguments->known_hosts->retrieve(arguments->known_hosts, i));
                    printf("\nSending %s : %d To -> %s: \n", tmp.ip, tmp.port , client_address);
                    //printStruct(tmp);
                    FullWrite(arguments->client, &tmp, sizeof(struct net_node));
                }
                //Inserimento in lista.
                for(int i = 0; i < 3; i++)
                {
                    memset(&tmp, 0, sizeof(struct net_node));             
                    FullRead(arguments->client, &tmp, sizeof(struct net_node));
                    //Cambio con strncpy
                    strncpy(tmp.ip, client_address, INET_ADDRSTRLEN);
                    pthread_mutex_lock(&file_mutex);
                    filePointer = fopen("peers.dat", "a+");
                    fwrite(&tmp, sizeof(struct net_node), 1, filePointer);
                    fclose(filePointer);
                    pthread_mutex_unlock(&file_mutex);
                    arguments->known_hosts->insert(arguments->known_hosts, arguments->known_hosts->length, &tmp, sizeof(struct net_node));
                }
            }

            //Il nodo è già in lista.
            else if(found == 1)
            {
                printf("\nNode: %s already added\n", client_address);
                //Indica al peer che è già in lista e di non dover inviare le sue informazioni.
                flag = -1;
                flag = htonl(flag);
                FullWrite(arguments->client, &flag, sizeof(int));
                //Creo la lista temporanea da cui rimuovere il peer richiedente.
                struct LinkedList tmp_list = linked_list_init();
                for(int i = 0; i < arguments->known_hosts->length; i++)
                {
                    tmp = (*(struct net_node*)arguments->known_hosts->retrieve(arguments->known_hosts, i));
                    tmp_list.insert(&tmp_list, i, &tmp, sizeof(struct net_node));
                }
        
                int tmp_len, removes = 0;
                tmp_len = tmp_list.length;

                //Dal momento in cui la lunghezza della lista è modificata a runtime servirà più di un'iterazione per la rimozione degli endpoint del peer.
                while(removes != 3)
                {
                    for(int i = 0; i < tmp_list.length; i++)
                    {
                        memset(&tmp, 0, sizeof(struct net_node));
                        tmp = (*(struct net_node*)tmp_list.retrieve(&tmp_list, i));
                        if(strncmp(tmp.ip, client_address, INET_ADDRSTRLEN) == 0)
                        {
                            //Rimuovo altrimenti il peer avrà se stesso in lista.
                            printf("\nRimosso: %s\n", client_address);
                            tmp_list.remove(&tmp_list, i);
                            removes++;
                        }
                    }
                }

                int newLen = tmp_list.length;
                newLen = htonl(newLen);
                FullWrite(arguments->client, &newLen, sizeof(int));
                newLen = ntohl(newLen);
                for(int i = 0; i < newLen; i++)
                {
                    memset(&tmp, 0, sizeof(struct net_node));
                    tmp = (*(struct net_node*) tmp_list.retrieve(&tmp_list, i));
                    printf("\nSending To -> %s: \n", client_address);
                    printStruct(tmp);
                    FullWrite(arguments->client, &tmp, sizeof(struct net_node));
                } 
                linked_list_destroy(&tmp_list);
            }
        }        
    close(arguments->client);
    }
}