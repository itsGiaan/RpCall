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
