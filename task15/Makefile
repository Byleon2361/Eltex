BIN := firstTask secondTask thirdTask
SRC := $(BIN:=.c)
OBJ := $(BIN:=.o)

CC = gcc
CFLAGS = -Wall

all: $(BIN)

%: %.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c 
	$(CC) $(CFLAGS) -c $^

clean:
	rm -rf *.o $(BIN)

.PHONY: all clean
