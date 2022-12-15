#include "peer.h"

int main(int argc, char** argv)
{
    struct net_node tmp;
    struct LinkedList known_nodes = linked_list_init();
    pthread_t thread1, thread2, thread3;
    int usage,choice;
    char server_ip[INET_ADDRSTRLEN];

    if(argc < 3)
    {
        perror("\nUsage: provide an integer 0 for node mode, 1 for user mode and server ip address\n");
        exit(EXIT_FAILURE);
    }
    fflush(stdin);

    usage = atoi(argv[1]);
    strcpy(server_ip, argv[2]);

    asciiART();

    if(!usage)
    {
        printf("\nNode mode selected\n");
        struct net_node my_info[3];

        //Riempimento delle struct rappresentanti gli endpoint.
        my_info[0].port = METHOD_PORT1;
        strcpy(my_info[0].name, "Add");
        strcpy(my_info[0].description, "This method adds two integers");
        strcpy(my_info[0].param1, "int");
        strcpy(my_info[0].param2, "int");

        my_info[1].port = METHOD_PORT2;
        strcpy(my_info[1].name, "subtract");
        strcpy(my_info[1].description, "This method subtract two floats");
        strcpy(my_info[1].param1, "float");
        strcpy(my_info[1].param2, "float");

        my_info[2].port = METHOD_PORT3;
        strcpy(my_info[2].name, "prod");
        strcpy(my_info[2].description, "This method return the product of two int");
        strcpy(my_info[2].param1, "int");
        strcpy(my_info[2].param2, "int");

        //Registrazione al server
        node_join_network(&known_nodes, my_info, usage, server_ip, SRV_PORT);

        //Creazione di tre thread in ascolto sulle porte dove sono hostate le procedure.
        //Ognuno provvederà ad accettare una nuova connessione creando un nuovo thread per gestirla.
        pthread_create(&thread1, NULL, server_function1, &known_nodes);
        pthread_create(&thread2, NULL, server_function2, &known_nodes);
        pthread_create(&thread3, NULL, server_function3, &known_nodes);

        while(1)
        {
            if(known_nodes.length > 0)
            {
                printList(&known_nodes);
                printf("\nChoose one of them: \n");
                scanf("%d", &choice);
                fflush(stdin);

                if(choice <= known_nodes.length)
                {
                    tmp = (*(struct net_node*) known_nodes.retrieve(&known_nodes, choice));
                    //Inoltro richiesta al nodo scelto.
                    node_request(tmp, usage, my_info);
                    sleep(10);
                }
                else
                {
                    printf("\nIncorrect index\n");
                    sleep(10);
                }
            }
            else
            {
                printf("\nNo nodes: %d \n", known_nodes.length);
                printf("\nWaiting for incoming connections\n");
                sleep(10);
            }
        }
    }

    else if(usage)
    {
        printf("\nUser mode selected\n");
        //Registrazione al server come utente.
        user_join_network(&known_nodes, usage, server_ip, SRV_PORT);
        while(1)
        {
            if(known_nodes.length > 0)
            {
                printList(&known_nodes);
                printf("\nChoose one of them: \n");
                scanf("%d", &choice);
                fflush(stdin);

                if(choice <= known_nodes.length)
                {
                    tmp = (*(struct net_node*) known_nodes.retrieve(&known_nodes, choice));
                    //Inoltro richiesta al nodo scelto.
                    user_request(tmp, usage);
                    sleep(10);
                }
                else
                {
                    printf("\nIncorrect index\n");
                    sleep(10);
                }
            }
            else
            {
                printf("\nNo nodes: %d \n", known_nodes.length);
                printf("\nRetry in 10 seconds\n");
                sleep(10);
                user_join_network(&known_nodes, usage, server_ip, SRV_PORT);
            }
        }
    }

    linked_list_destroy(&known_nodes);
    exit(EXIT_SUCCESS);
}