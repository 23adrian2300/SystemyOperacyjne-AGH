CC = gcc
FLAG = -Wall


all:
	make clean
	make compile_worker
	make compile_master
	make test

compile_worker:
	$(CC) $(FLAG) -o worker worker.c

compile_master:
	$(CC) $(FLAG) -o main main.c

test:
	./main 0.001 10
	./main 0.001 100
	./main 0.001 1000
	./main 0.0000000008 1050	

clean:
	rm -f worker main report.txt