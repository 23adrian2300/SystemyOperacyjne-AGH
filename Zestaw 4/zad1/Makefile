CC = gcc
FLAGS = -Wall


all:
	make clean
	make compile
	make test
compile:
	$(CC) $(FLAGS) exec.c -o exec.o
	$(CC) $(FLAGS) fork.c -o fork.o


test:
	echo "|FORK TEST|" > fork.txt
	echo "HANDLER:" >> fork.txt
	./fork.o handler >> fork.txt
	echo "IGNORE:" >>fork.txt
	./fork.o ignore >>fork.txt
	echo "MASK:" >>fork.txt
	./fork.o mask >>fork.txt
	echo "PENDING:" >>fork.txt
	./fork.o pending >>fork.txt

	echo "\n|TEST EXECL|" >>exec.txt
	echo "IGNORE:" >>exec.txt
	./exec.o ignore >>exec.txt
	echo "MASK:" >>exec.txt
	./exec.o mask >>exec.txt
	echo "PENDING:" >>exec.txt
	./exec.o pending >>exec.txt

clean:
	rm -f *.o exec.txt fork.txt


