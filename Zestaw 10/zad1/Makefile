CC = gcc
Flags = -Wall

all: 
	make clean
	make server
	make client

server:
	$(CC) $(Flags) server.c -o server -lpthread

client:
	$(CC) $(Flags) client.c -o client -lpthread

clean:
	rm -f server client