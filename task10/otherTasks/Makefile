
all: app1 app2 

app1: firstTask.o
	gcc -Wall -o app1 firstTask.o

firstTask.o: firstTask.c
	gcc -c firstTask.c

app2: secondTask.o
	gcc -Wall -o app2 secondTask.o

secondTask.o: secondTask.c
	gcc -c secondTask.c

clean:
	rm -rf app1 app2 *.o
