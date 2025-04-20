all: app

add.o:
	gcc -fPIC -c ./src/add.c

sub.o:
	gcc -fPIC -c ./src/sub.c

mul.o:
	gcc -fPIC -c ./src/mul.c

div.o:
	gcc -fPIC -c ./src/div.c

libcalc.so: add.o sub.o mul.o div.o
	gcc -shared -o libcalc.so $^

main.o:
	gcc -c main.c


app: libcalc.so main.o
	gcc main.o -L. -lcalc -o app -Wl,-rpath=.

run:
	./app

clean:
	rm app add.o sub.o mul.o div.o libcalc.so