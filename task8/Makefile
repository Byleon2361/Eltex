all: app

add.o:
	gcc -fPIC -c ./src/add.c

sub.o:
	gcc -fPIC -c ./src/sub.c

mul.o:
	gcc -fPIC -c ./src/mul.c

div.o:
	gcc -fPIC -c ./src/div.c

libdiv.so: div.o
	gcc -shared -o libdiv.so $^

libadd.so: add.o 
	gcc -shared -o libadd.so $^

libsub.so: sub.o
	gcc -shared -o libsub.so $^

libmul.so: mul.o
	gcc -shared -o libmul.so $^

main.o:
	gcc -c main.c


app: libdiv.so libadd.so libsub.so libmul.so main.o
	gcc main.o -L. -ladd -lsub -lmul -ldiv -o app -Wl,-rpath=.

run:
	./app

clean:
	rm -f app *.o *.so
