all:server client

server: server.o
	gcc -Wall -o server server.o -lrt

server.o: server.c
	gcc -c server.c

client: client.o
	gcc -Wall -o client client.o -lrt

client.o: client.c
	gcc -c client.c

clean:
	rm -rf server client *.o
