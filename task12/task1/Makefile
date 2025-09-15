all: app

app: pipe.o
	gcc -Wall -o app pipe.o

pipe.o: pipe.c
	gcc -c pipe.c

clean:
	rm -rf app *.o
