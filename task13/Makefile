all:server client

server: server.o nicknameManager.o
	gcc -Wall -o server server.o nicknameManager.o -lrt

server.o: server.c
	gcc -c -Wall server.c
	
nicknameManager.o: nicknameManager.c nicknameManager.h
	gcc -c -Wall nicknameManager.c

client: client.o
	gcc -Wall -o client client.o -lrt

client.o: client.c
	gcc -c -Wall client.c

clean:
	rm -rf server client *.o
