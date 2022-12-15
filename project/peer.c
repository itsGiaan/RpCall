#include "peer.h"

//For registering to the randezvous
void node_join_network(struct LinkedList *list, struct net_node *info, int mode, char *server_ip, int server_port)
{
   struct Client client = client_init(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   struct net_node tmp;
   int retVal, len;
   struct sockaddr_in serv_addr;
   socklen_t slen = (socklen_t)sizeof(serv_addr);

   serv_addr.sin_family = AF_INET;
   //serv_addr.sin_addr.s_addr = inet_addr(server_ip);
   serv_addr.sin_port = htons(server_port);
   Inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

   if((Connect(client.socket, (struct sockaddr*)&serv_addr, slen)) != -1)
   {
       printf("\nSuccessfully connected to %s\n", inet_ntoa(serv_addr.sin_addr));
       mode = htonl(mode);
       //Let the server know what kind of peer we are.
       FullWrite(client.socket, &mode, sizeof(int));
       //Get the list length so we know how much to loop.
       FullRead(client.socket, &retVal, sizeof(int));
       retVal = ntohl(retVal);
       //If the server list is empty give it our info first.
       if(retVal == 0)
       {
            for(int i = 0; i < 3; i++)
            {
                FullWrite(client.socket, &info[i], sizeof(struct net_node));
            }
       }
        //Il server non ha bisogno delle mie info.
        else if(retVal == -1)
        {
            FullRead(client.socket, &len, sizeof(int));
            len = ntohl(len);
            for(int i = 0; i < len; i++)
            {
                FullRead(client.socket, &tmp, sizeof(struct net_node));
                list->insert(list, i, &tmp, sizeof(struct net_node));
                printf("\nNode %s : %d added!\n", tmp.ip, tmp.port);
            }
        }

       else
       {
        //Loop for retVal times to store every peer knowed by the randezvous.
        for(int i = 0; i < retVal; i++)
        {
            FullRead(client.socket, &tmp, sizeof(struct net_node));
            list->insert(list, i, &tmp, sizeof(struct net_node));
            printf("\nNode %s : %d added\n", tmp.ip, tmp.port);
        }
        //Now we can pass our informations to get stored by randezvous.           
        for(int i = 0; i < 3; i++)
        {
            FullWrite(client.socket, &info[i], sizeof(struct net_node));
        }
       }
       
    }
    else
    {
        printf("\nUnable to connect to %s\n", inet_ntoa(serv_addr.sin_addr));
    }
   close(client.socket);
}



void * server_loop1(void *arg)
{
    struct ServerLoopArgument *arguments = arg;
    struct net_node tmp;
    struct attributes parameters;
    int len = arguments->known_hosts->length;
    int arg1, arg2, retVal, flag, usage;
    short found;
    char *client_address = inet_ntoa(arguments->server->address.sin_addr);
    char str_value[20];

    //What kind of peer are contacting me?
    FullRead(arguments->client, &usage, sizeof(int));
    usage = ntohl(usage);

    //It's a simple user.
    if(usage == 1)
    {
        printf("\nUser request\n");
        flag = 1;
        flag = htonl(flag);
        FullWrite(arguments->client, &flag, sizeof(int));
        printf("\nReady to accept parameters\n");
        //Reads and converts parameters to the right typo.
        FullRead(arguments->client, &parameters, sizeof(struct attributes));
        //printf("\nGot %s and %s\n", parameters.param1, parameters.param2);
        arg1 = atoi(parameters.param1);
        arg2 = atoi(parameters.param2);
        retVal = add(arg1, arg2);
        snprintf(str_value, 20, "%d\n", retVal);

        //Send back the result
        FullWrite(arguments->client, str_value, 20);
        //close(arguments->client);
    }
    
    //It's a node.
    else if(usage == 0)
    {
        printf("\nNode request\n");
        found = isInList(arguments->known_hosts, client_address);
        //If I don't know him, I have to.
        if(!len || !found)
        {
            //This means that I need his informations.
            flag = 0;
            flag = htonl(flag);
            FullWrite(arguments->client, &flag, sizeof(int));
            printf("\nNode info needed\n");

            //Get the peer info
            for(int i = 0; i < 3; i++)
            {
                memset(&tmp, 0, sizeof(struct net_node));
                FullRead(arguments->client, &tmp, sizeof(struct net_node));
                strcpy(tmp.ip, client_address);
                arguments->known_hosts->insert(arguments->known_hosts, arguments->known_hosts->length, &tmp, sizeof(struct net_node));
            }

            //Read and converts parameters
            printf("\nReady to read parameters\n");
            FullRead(arguments->client, &parameters, sizeof(struct attributes));
            arg1 = atoi(parameters.param1);
            arg2 = atoi(parameters.param2);
            retVal = add(arg1, arg2);
            //printf("\nResult: %d\n", retVal);
            snprintf(str_value, 20, "%d\n", retVal);

            //Send back the result
            FullWrite(arguments->client, str_value, 20);
            //close(arguments->client);
        }
        else
        {
            //Already know him, don't need any information.
            flag = 1;
            flag = htonl(flag);
            FullWrite(arguments->client, &flag, sizeof(int));

            //Read and converts parameters
            FullRead(arguments->client, &parameters, sizeof(struct attributes));
            arg1 = atoi(parameters.param1);
            arg2 = atoi(parameters.param2);
            retVal = add(arg1, arg2);
            //printf("\nResult: %d\n", retVal);
            snprintf(str_value, 20, "%d\n", retVal);

            //Send back the result
            FullWrite(arguments->client, str_value, 20);
            //close(arguments->client);
        }

    }
    close(arguments->client);
}


void * server_function1(void *arg)
{
    struct Server server = server_init(AF_INET, SOCK_STREAM, IPPROTO_TCP, INADDR_ANY, METHOD_PORT1, 20);
    socklen_t slen = (socklen_t)sizeof(server.address);
    pthread_t server_thread;

    while(1)
    {
        struct ServerLoopArgument loop_arg;
        loop_arg.client = Accept(server.socket, (struct sockaddr*)&server.address, &slen);
        loop_arg.server = &server;
        loop_arg.known_hosts = arg;

        pthread_create(&server_thread, NULL, server_loop1, &loop_arg);
    }
}


void * server_loop2(void *arg)
{
    struct ServerLoopArgument *arguments = arg;
    struct net_node tmp;
    struct attributes parameters;
    int len = arguments->known_hosts->length, flag, usage;
    float arg1_float, arg2_float, retVal_float;
    short found;
    char *client_address = inet_ntoa(arguments->server->address.sin_addr);
    char str_value[20];
    found = isInList(arguments->known_hosts, client_address);

    //What kind of peer are contacting me?
    FullRead(arguments->client, &usage, sizeof(int));
    usage = ntohl(usage);

    //It's a simple user.
    if(usage)
    {
        //Indica che si può procedere all'invio dei parametri
        flag = 1;
        flag = htonl(flag);
        FullWrite(arguments->client, &flag, sizeof(int));
        //Reads and converts parameters to the right typo.
        FullRead(arguments->client, &parameters, sizeof(struct attributes));
        arg1_float = atof(parameters.param1);
        arg2_float = atof(parameters.param2);
        retVal_float = difference(arg1_float, arg2_float);
        snprintf(str_value, 20, "%f\n", retVal_float);
        //Send back the result
        FullWrite(arguments->client, str_value, 20);
        close(arguments->client);
    }
    //It's a node.
    if(!usage)
    {
        if(!len || !found)
        {
            //Indica al peer che ho bisogno delle sue info.
            flag = 0;
            flag = htonl(flag);//Conversione in network order.
            FullWrite(arguments->client, &flag, sizeof(int));
            //Get the peer info
            for(int i = 0; i < 3; i++)
            {
                memset(&tmp, 0, sizeof(struct net_node));
                FullRead(arguments->client, &tmp, sizeof(struct net_node));
                strcpy(tmp.ip, client_address);
                arguments->known_hosts->insert(arguments->known_hosts, arguments->known_hosts->length, &tmp, sizeof(struct net_node));
            }
        }
        else
        {
            //Già sono a conoscenza non ho bisogno delle info
            flag = 1;
            flag = htonl(flag);//Conversione in network order.
            FullWrite(arguments->client, &flag, sizeof(int));
        }

        //Lettura e conversione parametri
        //printf("\nPronto alla lettura\n");
        FullRead(arguments->client, &parameters, sizeof(struct attributes));
        arg1_float = atof(parameters.param1);
        arg2_float = atof(parameters.param2);
        retVal_float = difference(arg1_float, arg2_float);
        snprintf(str_value, 20, "%f\n", retVal_float);
        
        //Send back the result
        FullWrite(arguments->client, str_value, 20);
        close(arguments->client);
    }
}


void * server_function2(void *arg)
{
    struct Server server = server_init(AF_INET, SOCK_STREAM, IPPROTO_TCP, INADDR_ANY, METHOD_PORT2, 20);
    socklen_t slen = (socklen_t)sizeof(server.address);
    pthread_t server_thread;

    while(1)
    {
        struct ServerLoopArgument loop_arg;
        loop_arg.client = Accept(server.socket, (struct sockaddr*)&server.address, &slen);
        loop_arg.server = &server;
        loop_arg.known_hosts = arg;

        pthread_create(&server_thread, NULL, server_loop2, &loop_arg);
    }
}


void * server_loop3(void *arg)
{
    struct ServerLoopArgument *arguments = arg;
    struct net_node tmp;
    struct attributes parameters;
    int len = arguments->known_hosts->length;
    int arg1, arg2, retVal, flag, usage;
    short found;
    char *client_address = inet_ntoa(arguments->server->address.sin_addr);
    char str_value[20];

    //What kind of peer are contacting me?
    FullRead(arguments->client, &usage, sizeof(int));
    usage = ntohl(usage);

    //It's a simple user.
    if(usage == 1)
    {
        printf("\nUser request\n");
        flag = 1;
        flag = htonl(flag);
        FullWrite(arguments->client, &flag, sizeof(int));
        //printf("\nReady to accept parameters\n");
        //Reads and converts parameters to the right typo.
        FullRead(arguments->client, &parameters, sizeof(struct attributes));
        //printf("\nGot %s and %s\n", parameters.param1, parameters.param2);
        arg1 = atoi(parameters.param1);
        arg2 = atoi(parameters.param2);
        retVal = prod(arg1, arg2);
        snprintf(str_value, 20, "%d\n", retVal);

        //Send back the result
        FullWrite(arguments->client, str_value, 20);
        //close(arguments->client);
    }
    
    //It's a node.
    else if(usage == 0)
    {
        printf("\nNode request\n");
        found = isInList(arguments->known_hosts, client_address);
        //If I don't know him, I have to.
        if(!len || !found)
        {
            //This means that I need his informations.
            flag = 0;
            flag = htonl(flag);
            FullWrite(arguments->client, &flag, sizeof(int));
            //printf("\nNode info needed\n");

            //Get the peer info
            for(int i = 0; i < 3; i++)
            {
                memset(&tmp, 0, sizeof(struct net_node));
                FullRead(arguments->client, &tmp, sizeof(struct net_node));
                strcpy(tmp.ip, client_address);
                arguments->known_hosts->insert(arguments->known_hosts, arguments->known_hosts->length, &tmp, sizeof(struct net_node));
            }

            //Read and converts parameters
            //printf("\nReady to read parameters\n");
            FullRead(arguments->client, &parameters, sizeof(struct attributes));
            arg1 = atoi(parameters.param1);
            arg2 = atoi(parameters.param2);
            retVal = prod(arg1, arg2);
            //printf("\nResult: %d\n", retVal);
            snprintf(str_value, 20, "%d\n", retVal);

            //Send back the result
            FullWrite(arguments->client, str_value, 20);
            //close(arguments->client);
        }
        else
        {
            //Already know him, don't need any information.
            flag = 1;
            flag = htonl(flag);
            FullWrite(arguments->client, &flag, sizeof(int));

            //Read and converts parameters
            FullRead(arguments->client, &parameters, sizeof(struct attributes));
            arg1 = atoi(parameters.param1);
            arg2 = atoi(parameters.param2);
            retVal = prod(arg1, arg2);
            //printf("\nResult: %d\n", retVal);
            snprintf(str_value, 20, "%d\n", retVal);

            //Send back the result
            FullWrite(arguments->client, str_value, 20);
            //close(arguments->client);
        }

    }
    close(arguments->client);
}


void * server_function3(void *arg)
{
    struct Server server = server_init(AF_INET, SOCK_STREAM, IPPROTO_TCP, INADDR_ANY, METHOD_PORT3, 20);
    socklen_t slen = (socklen_t)sizeof(server.address);
    pthread_t server_thread;

    while(1)
    {
        struct ServerLoopArgument loop_arg;
        loop_arg.client = Accept(server.socket, (struct sockaddr*)&server.address, &slen);
        loop_arg.server = &server;
        loop_arg.known_hosts = arg;

        pthread_create(&server_thread, NULL, server_loop3, &loop_arg);
    }
}


void node_request(struct net_node node, int mode, struct net_node *info)
{
    struct Client client = client_init(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int fd, retVal;
    struct attributes attr;
    struct sockaddr_in addr;
    char result[20];
    socklen_t slen = (socklen_t)sizeof(addr);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(node.port);
    Inet_pton(AF_INET, node.ip, &addr.sin_addr);

    if((Connect(client.socket, (struct sockaddr*)&addr, slen)) != -1)
    {
        mode = htonl(mode);
        FullWrite(client.socket, &mode, sizeof(int));

        FullRead(client.socket, &retVal, sizeof(int));
        retVal = ntohl(retVal);
        if(!retVal)
        {
            for(int i = 0; i < 3; i++)
            {
                FullWrite(client.socket, &info[i], sizeof(struct net_node));
            }
        }

        printf("\nInsert parameters -> %s %s one by one or divided by a space: \n", node.param1, node.param2);
        scanf("%s %s", attr.param1, attr.param2);

        //printf("\n1: %s 2: %s\n", attr.param1, attr.param2);
        
        FullWrite(client.socket, &attr, sizeof(struct attributes));
        printf("\nParameters sended!\n");

        FullRead(client.socket, result, 20);
        setResult(node, result);
    }
    close(client.socket);
}


void user_join_network(struct LinkedList *list, int mode, char *server_ip, int server_port)
{
   struct Client client = client_init(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   struct net_node tmp;
   int retVal;
   struct sockaddr_in serv_addr;
   socklen_t slen = (socklen_t)sizeof(serv_addr);

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(server_port);
   Inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

   if((Connect(client.socket, (struct sockaddr*)&serv_addr, slen)) != -1)
   {
       printf("\nSuccessfully connected to %s\n", inet_ntoa(serv_addr.sin_addr));
       mode = htonl(mode);
       FullWrite(client.socket, &mode, sizeof(int));
       //Get the list length so we know how much to loop
       FullRead(client.socket, &retVal, sizeof(int));
       retVal = ntohl(retVal);
       if(retVal == -1 || !retVal)
       {
           printf("\nNo node available, retry in 10 seconds\n");
           sleep(10);
           user_join_network(list, mode, server_ip, server_port);
       }
       else
       {
           for(int i = 0; i < retVal; i++)
           {
               FullRead(client.socket, &tmp, sizeof(struct net_node));
               list->insert(list, list->length, &tmp, sizeof(struct net_node));
               printf("\nNode %s : %d added\n", tmp.ip, tmp.port);
           }
       }
       close(client.socket);
    }
    else
    {
        printf("\nUnable to connect to %s\n", inet_ntoa(serv_addr.sin_addr));
        close(client.socket);
        exit(EXIT_FAILURE);
    }
}


void user_request(struct net_node node, int mode)
{
    struct Client client = client_init(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int fd, retVal;
    struct attributes attr;
    struct sockaddr_in addr;
    char result[20];
    socklen_t slen = (socklen_t)sizeof(addr);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(node.port);
    Inet_pton(AF_INET, node.ip, &addr.sin_addr);

    if((Connect(client.socket, (struct sockaddr*)&addr, slen)) != -1)
    {
        mode = htonl(mode);
        FullWrite(client.socket, &mode, sizeof(int));

        FullRead(client.socket, &retVal, sizeof(int));
        retVal = ntohl(retVal);
        if(!retVal)
        {
            printf("\nTry later\n");
            close(client.socket);
        }

        printf("\nInsert parameters -> %s %s one by one or divided by a space: \n", node.param1, node.param2);
        scanf("%s %s", attr.param1, attr.param2);
        
        FullWrite(client.socket, &attr, sizeof(struct attributes));
        printf("\nParameters sended!\n");

        FullRead(client.socket, result, 20);
        setResult(node, result);
    }
    else
    {
        printf("\nHost %s unreachable\n", inet_ntoa(addr.sin_addr));
        exit(EXIT_FAILURE);
    }
    close(client.socket);
}