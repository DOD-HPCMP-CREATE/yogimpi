CC=mpicc
CFLAGS=-I. -fPIC
LDFLAGS=-shared

.PHONY: default clean test

default: yogimpi.c yogimpi.h
	$(CC) $(CFLAGS) -c yogimpi.c
	$(CC) $(CFLAGS) $(LDFLAGS) yogimpi.o -o libyogimpi.so

clean:
	$(RM) *.o *.so nonblock sendrecv testMatt

test: test/test_NonBlocking.c test/test_SendRecv.c test/test_Matt.c default 
	$(CC) $(CFLAGS) -L. -lyogimpi test/test_NonBlocking.c -o nonblock
	$(CC) $(CFLAGS) -L. -lyogimpi test/test_SendRecv.c -o sendrecv
	$(CC) $(CFLAGS) -L. -lyogimpi test/test_Matt.c -o testMatt 
