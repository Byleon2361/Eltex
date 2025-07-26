#!/bin/bash

SERVERS=("simpleParallelTcpServer" "simpleParallelUdpServer" "poolThreadsTcpServer" "poolThreadsUdpServer" "producerCustomerTcpServer" "producerCustomerUdpServer" "multiProtocolServer")

CLIENTS=("clientTcp" "clientUdp")

CLIENT_COUNTS=(1000 10000 100000)

TESTER="./tester"
if [ ! -f "$TESTER" ]; then
    echo "Error: Tester program ($TESTER) not found"
    exit 1
fi

> "test.txt"

for server in "${SERVERS[@]}"; do
    if [[ $server == *"Tcp"* ]]; then
        client="clientTcp"
    else
        client="clientUdp"
    fi

    if [ ! -f "./$server" ]; then
        echo "Error: Server program ($server) not found"
        continue
    fi
    if [ ! -f "./$client" ]; then
        echo "Error: Client program ($client) not found"
        continue
    fi

    for count in "${CLIENT_COUNTS[@]}"; do
        echo "Running test: $server with $client for $count clients"
        echo "Test: $server with $client for $count clients" 
        $TESTER "$server" "$client" "$count" >> /dev/null
        if [ $? -eq 0 ]; then
            echo "Test completed successfully"
        else
            echo "Test failed"
        fi
        echo "--------------------------------"
    done
done
