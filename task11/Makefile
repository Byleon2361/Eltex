all: app

app: customer.o loader.o main.o
	gcc $^ -o app -pthread

customer.o: customer.c customer.h
	gcc -c customer.c

loader.o: loader.c loader.h
	gcc -c loader.c

main.o: main.c
	gcc -c main.c

clean:
	rm -rf app main.o customer.o loader.o
