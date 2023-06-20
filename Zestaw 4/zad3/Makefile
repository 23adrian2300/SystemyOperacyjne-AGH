CC = gcc
FLAGS = -Wall


all:
	make clean
	make compile2
	make compile1

compile1:
	$(CC) $(FLAGS) catcher.c -o catcher.o

compile2:
	$(CC) $(FLAGS) sender.c -o sender.o

clean:
	rm -f *.o