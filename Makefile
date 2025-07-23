CC = gcc
CFLAGS = -pthread -Wall

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f server client /tmp/server_fifo /tmp/client_fifo_*
