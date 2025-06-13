all: app1 app2 app3

app1: firstTask.o
	gcc -Wall -o app1 firstTask.o

firstTask.o: firstTask.c
	gcc -c firstTask.c

app2: secondTask.o
	gcc -Wall -o app2 secondTask.o

secondTask.o: secondTask.c
	gcc -c secondTask.c

app3: thirdTask.o
	gcc -Wall -o app3 thirdTask.o

thirdTask.o: thirdTask.c
	gcc -c thirdTask.c

clean:
	rm -rf app1 app2 app3 *.o
