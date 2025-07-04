all:server client

server: server.o 
	gcc -Wall -o server server.o -lrt

server.o: server.c
	gcc -c -Wall server.c

client: client.o view.o
	gcc -Wall -o client client.o view.o -lrt -lcurses

client.o: client.c
	gcc -c -Wall client.c

view.o: view.c view.h
	gcc -c -Wall view.c

clear:
	rm -rf /dev/mqueue/*

clean:
	rm -rf server client *.o
