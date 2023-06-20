CC = gcc
CFLAGS = -Wall
LIB = wc_library
OPT = -O0

all:
	make clean
	make static_library
	cat test | ./REPL_static.exe >> report_static_O0.txt

	make clean
	make static_library
	cat test | ./REPL_static.exe >> report_static_O1.txt

	make clean
	make static_library
	cat test | ./REPL_static.exe >> report_static_Os.txt

	make clean
	make shared_library
	cat test | ./REPL_shared.exe >> report_shared_O0.txt

	make clean
	make shared_library
	cat test | ./REPL_shared.exe >> report_shared_O1.txt

	make clean
	make shared_library
	cat test | ./REPL_shared.exe >> report_shared_Os.txt

	make clean
	make dynamic_library
	cat test | ./REPL_dynamic.exe >> report_dynamic_O0.txt

	make clean
	make dynamic_library
	cat test | ./REPL_dynamic.exe >> report_dynamic_O1.txt

	make clean
	make dynamic_library
	cat test | ./REPL_dynamic.exe >> report_dynamic_Os.txt

export LD_LIBRARY_PATH=.

static_library: ${LIB}.o
	ar rcs libwc_library.a wc_library.o
	${CC} ${CFLAGS} ${OPT} REPL_wc_library.c -o REPL_static.exe -L. -l${LIB}


dynamic_library: ${LIB}.o
	${CC} ${CFLAGS} ${OPT} -shared -fPIC -o lib${LIB}.so ${LIB}.o
	${CC} ${CFLAGS} ${OPT} REPL_wc_library.c -o REPL_dynamic.exe -L. -ldl -D DLL
	

shared_library: ${LIB}.o
	export LD_LIBRARY_PATH= pwd
	${CC} ${CFLAGS} ${OPT} -shared -fPIC -o lib${LIB}.so ${LIB}.o
	${CC} ${CFLAGS} ${OPT} REPL_wc_library.c -o REPL_shared.exe -L. -l${LIB}
	

${LIB}.o: ${LIB}.c
	${CC} ${CFLAGS} ${OPT} -c ${LIB}.c
	
test_static_library:
	make clean
	make static_library
	cat test | ./REPL_static.exe >> report_static_${OPT}.txt


test_shared_library:
	make clean
	make shared_library
	cat test | ./REPL_shared.exe >> report_shared_${OPT}.txt


test_dynamic_library:
	make clean
	make DLL_library
	cat test | ./REPL_dynamic.exe >> report_dynamic_${OPT}.txt

clean:
	rm -f *.o *.gch *.a *.so 

clean_all:
	rm -f *.o *.gch *.a *.so *.txt *.exe