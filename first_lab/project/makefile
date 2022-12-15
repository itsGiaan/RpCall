all: randezvous peer

randezvous: mainServer.c LinkedList.c Node.c server.c utils.c wrapper.c
	gcc -o randezvous mainServer.c LinkedList.c Node.c server.c utils.c wrapper.c -lpthread 

peer: mainPeer.c LinkedList.c Node.c server.c client.c utils.c wrapper.c peer.c
	gcc -o peer mainPeer.c LinkedList.c Node.c server.c client.c utils.c wrapper.c peer.c -lpthread
