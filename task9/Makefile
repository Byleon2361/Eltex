all: app

app: mc.o main.o
	gcc mc.o main.o -o app -lncurses

mc.o: mc.c mc.h
	gcc -c mc.c

main.o: main.c
	gcc -c main.c

clean:
	rm -rf app main.o mc.o
