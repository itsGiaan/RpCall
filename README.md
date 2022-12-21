# RpCall
An hybrid p2p network to implement a remote procedure caller built for a networking class in 2021

# Overview
Design a p2p application to implement the remote procedure call service.
Every peer makes a certain number (>2) of procedures available to others peers on the network that can be called remotely.
To join the network, each peer must register with the server (Randezvous) by sending it the list of procedures it makes available, the arguments of each procedure and the port number on wich each procedure is available.
Each peer allows the user to call a remote procedure maded available by another peer. 
To do this, it retrives the list of remote procedures from the server and according to the user's choice, connects to the peer selected and sending the requested arguments.
The receiver peer performs the requested procedure and return the result, wich will showed to the user.

# Project description
The applications includes 2 main parts:
- Randezvous: plays the role of intermediary between the partecipants of the network so they can find each other.
              All it does is accept new connections and save the details of the various endpoint made available 
              by the pertecipants, supplying them from time to time to the new ones.

In order to make the application usable even by those peers who do not have methods to make available, it was decided to implement a differentiation between the types of peers, to do so, when the application will start, the user will be asked for the role to log in, wich can be of two types:

- Node: The active peer, to become one, it will have to register to the Randezvous server, obtaining data of the various
        participants and providing the informations about his own endpoints where the related procedures are hosted.
        Each of them will be able to use the procedures maded available from other nodes.
        
- User: likewise to the nodes, they will perform a connection to the server to obtain the endpoint's list with relative procedures
        offered and been able to use them too. However, they will not become active partecipants in the network since they don't
        provide any method, and the informations about this part will be left out.

# Network architecture
![image](https://user-images.githubusercontent.com/120039903/208430358-a9f5fee6-5d01-45ec-a4c0-e389a5d4be82.png)
The network architecture is based on an hybrid p2p model which provides the presence of a central server to facilitate the
exchange of endpoints between partecipants. In this case, the Randezvous server will play the role of meeting point and
although it represent an SPF (Single Point of Failure) in the event of a crash or sudden disconnection, the participants will
be able to continue to offer/use remote procedures in a completely decentralized way, but a new participant won't be able
to join the network.

# Application Protocol
When starting a peer, it will be necessary to provide it an integer (0 or 1) to choose the access mode: 
0 for Node-mode, 1 for User-mode and the server's ip address.
The identification of the nodes will be done through the following struct net_node:

![image](https://user-images.githubusercontent.com/120039903/208434226-faa4957c-fae9-4072-aa6d-66d9eab2a8b4.png)

# Network join phase
- Node mode case: The node will start filling the fields of its struct net_node representing the endpoints and informations
on the methods made available. In particular, the port number, name and description of the method and, assuming that all 
procedures takes two arguments as inputs, two strings representig the type of parameters accepted. The field reserved for the
ip addess, on the other hand, will be filled by the server in case of registration or by a peer receving a request from an 
unknown node (entered the network at a later time and not present in the list provided by the Randezvous.
The node will make the connection to the randezvous server in a completely sequential way providing it with the mode type which,
in this case, will be 0 so that the server knows that it is a node and not a user, and, if it is necessary to add it to the list, 
this will happen only if the peer is a node and the ip address is unknown to the server itself, 
otherwise it will go directly to sending the list. 
Once the nature of the peer has been ascertained, the server, if there are already other nodes, will provide an integer 
representing the length of the list so that the peer knows how many times to iterate the reading of the net_node packet. 
Otherwise, not having endpoints in the list, the server will send an integer =0, so that the node waits for new requests.
At the end of this step, the peer will send its information via three packets of the net_node type (one for each procedure offered)
which the server will add to its list. In case the peer connects to the server again after a disconnection, the server will detect 
the presence of the peer in its list and will to create a temporary version in which the latter is not present and,
following an approach similar to the previous case, it will communicate to the peer an integer representing the length of the new temporary list, 
repeating the sending of the same and subsequently destroying it.
At this point the peer is to all intents and purposes an active participant in the network, which can use the procedures offered by the other 
participants and offer its own, communicating in a completely decentralized way with other nodes.

- User mode case:
The user will play a passive participant role as they will have nothing to offer to the network. In this case, in fact, the registration phase to the server will differ from that of the node only as regards the sending of its information, which will be omitted. After obtaining the endpoints of the participants from the randezvous server, the user will be able to use all the procedures known to him and available at that moment.

# Methods hosting
Each of the nodes will make 3 procedures available to the network, each of them respectively on ports 4443, 4444, 4445.
When a request occur, the node will check the nature of the calling peer by verifying whether it is another node or a user.
In the case of a user, he will provide an integer = 1 to communicate that he can send the parameters that will be passed to the method of interest. These parameters will be passed in the form of strings, this is because C does not support dynamic data types and, for better modularity, strings offer an excellent compromise, it will then be the node that converts the strings into the right type. Once received and converted, the parameters will be passed as arguments to the method and the result will be written in a string representing the output of the procedure which the receiver will convert into the right type, as occurs for receiving parameters. Finally, the value obtained from the node will be shown on the screen.
If the request arrives from another node, two scenarios can occur:
- If the node is unknown or is the first to contact us, we will pass it an integer = 0, this will indicate to the node that we will need its information to populate the list. Only then will it be possible to move on to the phase of receiving the parameters and sending the result.
- If the node is already in the list we will pass it, as in the case of the user, an integer = 1 which will indicate the willingness to go directly to the phase of sending the parameters.

# Server implementation details
The Randezvous initializes a server-type object that represents a wrapper for a Listen Socket that will be used to accept new connections, it will load the peers.dat file which is nothing more than a backup copy of the list of network endpoints, and , if there were any saved from a previous execution, it will initialize a LinkedList with them; otherwise, the list will be created and will wait for a new connection to populate it. It has been chosen to implement a LinkedList of pointers to void in order to have a sort of dynamic array template that supports insertion, extraction, removal methods and an internal index to the structure updated at runtime that represents its size.
The main thread will be dedicated exclusively to accepting connections from peers and, for each new connection, will fill the fields of the ServerLoopArgument struct:

![image](https://user-images.githubusercontent.com/120039903/208464463-bab8b6bc-9bfe-421c-bd50-98e2bca8a941.png)

Which respectively represent:
- client: the descriptor returned by accept().
- *server: pointer to the struct server initialized at the start of the Randezvous, to retrieve the address of the peer
  who contacted it.
- *known_nodes: pointer to the LinkedList where we will save the informations of various peers.

Once this is done, the ServerLoopArgument type object will be passed as a parameter to the thread in charge of handling the request.

# Peer implementation details
As already described above, when the peer starts, it will need an integer (0 or 1) passed as a command line argument: this will serve to differentiate nodes from users, making the network open even to those who do not have a method to make available.
The user routine provides for the initialization of the LinkedList, a first connection to the server, the retrieval of the endpoints if there are any, otherwise it will be necessary to try to reconnect at a later time; after which, the user can use any of the procedures made available by the nodes known to him by making a request (user_request()), all in a sequential manner since he himself will contact the others and never be contacted.
As for the nodes, a LinkedList will be initialized which will host the endpoints received from the server and from the unknown nodes that have contacted us, after which 3 threads will be created, each of which will be in charge of accepting a connection and in a similar way to what happens with the I /O multiplexing by creating a new thread passing it the struct ServerLoopArgument to handle this request only when necessary, in a completely analogous way to what happened in the server.
The main thread will take care of all the part relating to the use of remote methods and sending parameters. Therefore, all the endpoints in the list will be printed on the screen and it will be possible to choose one of them, extract it from the LinkedList to obtain the data and make a request (node_request()).

# How to run it
- Compilation: 
To compile it will be sufficient to go to the /rpCall directory and launch the make command, the executables will be generated to be launched with ./randezvous and ./peer.
When starting the peer, you will need to provide it with an integer (0 or 1) to choose the access mode, remember, 0 = node_mode and 1 = user_mode, followed by the randezvous ip address.
- Execution: We will be using Katharà, a network environment emulator that requires the following installation steps:
- Install Docker (https://docs.docker.com/engine/install/).
- Install xterm terminal emulator (sudo apt install xterm or sudo yum install xterm or sudo pacman -S xterm).
- Install the right version of Kathara (https://github.com/KatharaFramework/Kathara/wiki/Installation-Guides).


Once all the components are installed, just go to the /lab folder and launch the kathara lstart command. Once the devices have been created, from each of them, move to the /shared/project directory where the sources will be present.
Run the make command from the randezvous device and then ./randezvous,
from other devices ./peer <method of use> <ip of randezvous>.
To close the laboratory and related devices, simply run the kathara wipe command.
