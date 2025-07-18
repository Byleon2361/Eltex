BIN := tester simpleParallelTcpServer simpleParallelTcpClient simpleParallelUdpServer simpleParallelUdpClient
SRC := $(BIN:=.c)
OBJ := $(BIN:=.o)

CC = gcc
CFLAGS = -Wall

all: $(BIN)

$(BIN): %: %.o
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ): %.o: %.c 
	$(CC) $(CFLAGS) -c $^

clean:
	rm *.o $(BIN)

.PHONY: all clean
