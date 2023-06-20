
CC = gcc
CFLAGS = -Wall
LIB = wc_library

all:
	make clean
	make static_library
	cat test | ./REPL_static.exe >> report_static.txt


	make clean
	make shared_library
	cat test | ./REPL_shared.exe >> report_shared.txt

	make clean
	make dynamic_library
	cat test | ./REPL_dynamic.exe >> report_dynamic.txt


export LD_LIBRARY_PATH=.

static_library: ${LIB}.o
	ar rcs libwc_library.a wc_library.o
	${CC} ${CFLAGS} REPL_wc_library.c -o REPL_static.exe -L. -l${LIB}


dynamic_library: ${LIB}.o
	${CC} ${CFLAGS} -shared -fPIC -o lib${LIB}.so ${LIB}.o
	${CC} ${CFLAGS} REPL_wc_library.c -o REPL_dynamic.exe -L. -ldl -D DLL
	

shared_library: ${LIB}.o
	export LD_LIBRARY_PATH=pwd
	${CC} ${CFLAGS} -shared -fPIC -o lib${LIB}.so ${LIB}.o
	${CC} ${CFLAGS} REPL_wc_library.c -o REPL_shared.exe -L. -l${LIB}
	

${LIB}.o: ${LIB}.c
	${CC} ${CFLAGS} -c ${LIB}.c
	
test_static_library:
	make clean
	make static_library
	cat test | ./REPL_static >> report_static.txt


test_shared_library:
	make clean
	make shared_library
	cat test | ./REPL_shared >> report_shared.txt


test_dynamic_library:
	make clean
	make dynamic_library
	cat test | ./REPL_dynamic >> report_dynamic.txt

clean:
	rm -f *.o *.gch *.a *.so 

clean_all:
	rm -f *.o *.gch *.a *.so *.txt *.exe
