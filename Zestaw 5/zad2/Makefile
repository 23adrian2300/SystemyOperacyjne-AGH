CC = gcc
FLAG = -Wall


all:
	make clean
	make compile
	make test

compile:
	$(CC) $(FLAG) main.c -o zad2

test:
	./zad2 0.001 10
	./zad2 0.001 100
	./zad2 0.001 1000
	./zad2 0.0000000008 1000

clean:
	rm -f zad2 report.txt
