BIN := localTcpClient localTcpServer localUdpClient localUdpServer netTcpClient netTcpServer netUdpClient netUdpServer 
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
	rm -rf *.o $(BIN)

.PHONY: all clean
