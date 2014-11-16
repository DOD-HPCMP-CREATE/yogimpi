MPICC=mpicc
CC=gcc
MPIF90=mpif90
F90=gfortran
FFLAGS=-I. -fPIC
CFLAGS=-I. -fPIC
LDFLAGS=-shared

.PHONY: default clean test

default: yogimpi.c yogimpi_f90bridge.c yogimpi.h
	$(MPICC) $(CFLAGS) -c yogimpi.c
	$(MPICC) $(CFLAGS) -c yogimpi_f90bridge.c
	$(MPICC) $(CFLAGS) $(LDFLAGS) yogimpi.o yogimpi_f90bridge.o \
              -o libyogimpi.so

clean:
	$(RM) *.o *.so nonblock sendrecv testMatt fsendrecv ftestMatt simple

test: test/test_NonBlocking.c test/test_SendRecv.c test/test_Matt.c default 
	$(CC) $(CFLAGS) test/test_StartupShutdown.c -L. -lyogimpi -o simple
	$(CC) $(CFLAGS) test/test_NonBlocking.c -L. -lyogimpi -o nonblock
	$(CC) $(CFLAGS) test/test_SendRecv.c -L. -lyogimpi -o sendrecv
	$(CC) $(CFLAGS) test/test_Matt.c -L. -lyogimpi -o testMatt 
	$(F90) $(FFLAGS) test/ftest_SendRecv.f90 -L. -lyogimpi -o fsendrecv 
	$(F90) $(FFLAGS) test/ftest_Matt.f90 -L. -lyogimpi -o ftestMatt

