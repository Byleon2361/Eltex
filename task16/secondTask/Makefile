BIN := tester clientTcp clientUdp simpleParallelTcpServer simpleParallelUdpServer poolThreadsTcpServer poolThreadsUdpServer producerCustomerTcpServer producerCustomerUdpServer multiProtocolServer
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
