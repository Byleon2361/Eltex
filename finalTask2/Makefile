all: app

app: driver.o main.o
	gcc $^ -o app 

mcVisual.o: driver.c driver.h
	gcc -c driver.c

main.o: main.c
	gcc -c main.c

clean:
	rm -rf app main.o driver.o
