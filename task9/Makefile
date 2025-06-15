all: app

app: mcVisual.o mcControl.o main.o
	gcc $^ -o app -lncurses

mcVisual.o: mcVisual.c mcVisual.h
	gcc -c mcVisual.c

mcControl.o: mcControl.c mcControl.h
	gcc -c mcControl.c

main.o: main.c
	gcc -c main.c

clean:
	rm -rf app main.o mcVisual.o mcControl.o
