BIN := server client
SRC := $(BIN:=.c) rawUdp.c
OBJ := $(BIN:=.o) rawUdp.o

CC = gcc
CFLAGS = -Wall

all: $(BIN)

$(BIN): %: %.o rawUdp.o
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ): %.o: %.c 
	$(CC) $(CFLAGS) -c $^

clean:
	rm *.o $(BIN)

.PHONY: all clean
