CC = gcc
CFLAGS = -Wall -Wextra

all: 
	make clean
	make compile

compile:
	$(CC) $(CFLAGS) main.c -o main

test:
	./main 10

clean:
	rm -f main *.txt