CC=mpicc
F90=mpif90
FFLAGS=-I. -fPIC
CFLAGS=-I. -fPIC
LDFLAGS=-shared

.PHONY: default clean test

default: yogimpi.c yogimpi_f90bridge.c yogimpi.h
	$(CC) $(CFLAGS) -c yogimpi.c
	$(CC) $(CFLAGS) -c yogimpi_f90bridge.c
	$(CC) $(CFLAGS) $(LDFLAGS) yogimpi.o yogimpi_f90bridge.o \
              -o libyogimpi.so

clean:
	$(RM) *.o *.so nonblock sendrecv testMatt fsendrecv ftestMatt

test: test/test_NonBlocking.c test/test_SendRecv.c test/test_Matt.c default 
	$(CC) $(CFLAGS) -L. -lyogimpi test/test_NonBlocking.c -o nonblock
	$(CC) $(CFLAGS) -L. -lyogimpi test/test_SendRecv.c -o sendrecv
	$(CC) $(CFLAGS) -L. -lyogimpi test/test_Matt.c -o testMatt 
	$(F90) $(FFLAGS) -L. -lyogimpi test/ftest_SendRecv.f90 -o fsendrecv 
	$(F90) $(FFLAGS) -L. -lyogimpi test/ftest_Matt.f90 -o ftestMatt

