all: app server client

app: pipe.o
	gcc -Wall -o app pipe.o

pipe.o: pipe.c
	gcc -c pipe.c

server: server.o
	gcc -Wall -o server server.o

server.o: server.c
	gcc -c server.c

client: client.o
	gcc -Wall -o client client.o

client.o: client.c
	gcc -c client.c

clear:
	rm pipe

clean:
	rm -rf app server client *.o
