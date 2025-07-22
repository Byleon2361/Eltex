BIN := broadcastSender broadcastReceiver multicastSender multicastReceiver
SRC := $(BIN:=.c)
OBJ := $(BIN:=.o)

CC = gcc
CFLAGS = -Wall

all: $(BIN)

$(BIN): %: %.o
	$(CC) $(CFLAGS) -o $@ $^ -pthread

$(OBJ): %.o: %.c 
	$(CC) $(CFLAGS) -c $^ -pthread

clean:
	rm *.o $(BIN)

.PHONY: all clean
