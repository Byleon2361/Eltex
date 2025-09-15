all: comInter

comInter: comInter.o main.o
	gcc -Wall -o comInter comInter.o main.o

comInter.o: comInter.c comInter.h
	gcc -c -g -O0 comInter.c

main.o: main.c
	gcc -c -g -O0 main.c
	
run: comInter
	./comInter

clean:
	rm -rf comInter *.o
