CC = gcc
CFLAGS = -Wall 

all:  
	make clean
	make compile


compile:
	$(CC) $(CFLAGS) main.c -o main

test1:
	./main . "#include"
	@echo "\n"
test2:
	./main .. "#include"
	@echo "\n"

test3:
	./main ../.. "#include"
	@echo "\n"
test4:
	./main . "asdf"
	@echo "\n"
test5:

	./main .. "asdf"
	@echo "\n"

test6:
	./main ../.. "asdf"
	@echo "\n"

test7:
	./main . "zadanie"
	@echo "\n"

test8:
	./main .. "zadanie"
	@echo "\n"

test9:
	./main ../.. "zadanie"
	@echo "\n"

test10:
	./main ~ "#include"
	@echo "\n"
	
tests:
	make test1
	make test2
	make test3
	make test4
	make test5
	make test6
	make test7
	make test8
	make test9


clean:
	rm -rf *.o main



