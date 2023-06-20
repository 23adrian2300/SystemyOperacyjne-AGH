
CC = gcc
CFLAGS = -Wall
LIB = wc_library

all:
	make clean
	make static_library

	make clean
	make shared_library

	make clean
	make DLL_library



static_library: ${LIB}.o
	ar rcs libwc_library.a wc_library.o
	${CC} ${CFLAGS} REPL_wc_library.c -o REPL_static.exe -L. -l${LIB}


DLL_library: ${LIB}.o
	${CC} ${CFLAGS} -shared -fPIC -o lib${LIB}.so ${LIB}.o
	${CC} ${CFLAGS} REPL_wc_library.c -o REPL_DLL.exe -L. -ldl -D DLL
	

shared_library: ${LIB}.o
	export LD_LIBRARY_PATH=pwd
	${CC} ${CFLAGS} -shared -fPIC -o lib${LIB}.so ${LIB}.o
	${CC} ${CFLAGS} REPL_wc_library.c -o REPL_shared.exe -L. -l${LIB}
	

${LIB}.o: ${LIB}.c
	${CC} ${CFLAGS} -c ${LIB}.c
	

clean:
	rm -f *.o *.gch *.a *.so 

clean_all:
	rm -f *.o *.gch *.a *.so *.exe
