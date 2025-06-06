all: app

add.o:
	gcc -fPIC -c ./src/add.c

sub.o:
	gcc -fPIC -c ./src/sub.c

mul.o:
	gcc -fPIC -c ./src/mul.c

div.o:
	gcc -fPIC -c ./src/div.c

libmydiv.so: div.o
	gcc -shared -o libmydiv.so $^

libmyadd.so: add.o 
	gcc -shared -o libmyadd.so $^

libmysub.so: sub.o
	gcc -shared -o libmysub.so $^

libmymul.so: mul.o
	gcc -shared -o libmymul.so $^

main.o:
	gcc -c main.c


app: libmydiv.so libmyadd.so libmysub.so libmymul.so main.o
	gcc main.o -L. -lmyadd -lmysub -lmymul -lmydiv -o app -Wl,-rpath=.

run:
	./app

clean:
	rm -f app *.o *.so
