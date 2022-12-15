#ifndef peer_h
#define peer_h

#include "LinkedList.h"
#include "utils.h"
#include "server.h"
#include "client.h"
#include "wrapper.h"
#include <pthread.h>

#define SRV_PORT 9930 
#define METHOD_PORT1 4444
#define METHOD_PORT2 4443
#define METHOD_PORT3 4445

//To join the network as a node.
void node_join_network(struct LinkedList *list, struct net_node *info, int mode, char *server_ip, int server_port);

//The function to execute from the first thread once a new connection is accepted.
void * server_loop1(void *arg);

//The function executed from the first thread to accept new connection.
void * server_function1(void *arg);

//The function to execute from the second thread once a new connection is accepted.
void * server_loop2(void *arg);

//The function executed from the second thread to accept new connection.
void * server_function2(void *arg);

//The function to execute from the second thread once a new connection is accepted.
void * server_loop3(void *arg);

//The function executed from the second thread to accept new connection.
void * server_function3(void *arg);

//Function to request a remote procedure from a chosen peer.
void node_request(struct net_node node, int mode, struct net_node *info);

//To join the network as a user.
void user_join_network(struct LinkedList *list, int mode, char *server_ip, int server_port);

//Function to execute a remote procedure as a user.
void user_request(struct net_node node, int mode);

#endif