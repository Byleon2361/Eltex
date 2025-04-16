all: app


add.o:
	gcc -c ./src/add.c

sub.o:
	gcc -c ./src/sub.c

mul.o:
	gcc -c ./src/mul.c

div.o:
	gcc -c ./src/div.c

libcalc.a: add.o sub.o mul.o div.o
	ar r libcalc.a $^

app: libcalc.a
	gcc main.c -L. -lcalc -o app

run:
	./app 

clean:
	rm app add.o sub.o mul.o div.o libcalc.a