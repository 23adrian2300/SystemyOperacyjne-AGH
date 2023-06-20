CC=gcc
CFLAGS=-Wall
LIB=wc_library


all:
	make static_library
	make shared_library


static_library:  ${LIB}.o
	${CC} ${CFLAGS} -c ${LIB}.c
	ar rcs lib${LIB}.a ${LIB}.o

shared_library: $(LIB).c
	export LD_LIBRARY_PATH=pwd
	$(CC) $(CFLAGS) -c -fPIC $(LIB).c
	$(CC) $(CFLAGS) -shared -o $(LIB).so $(LIB).o

${LIB}.o: ${LIB}.c
	${CC} ${CFLAGS} -c ${LIB}.c

clean:
	rm -f *.o *.gch *.a *.so
