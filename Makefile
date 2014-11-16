MPICC=mpicc
#CC=gcc-mp-4.8
CC=gcc
MPIF90=mpif90
#F90=gfortran-mp-4.8
F90=gfortran
FFLAGS=-cpp -I. -fPIC
CFLAGS=-I. -fPIC
LDFLAGS=-shared

.PHONY: default clean test

default: yogimpi.c yogimpi_f90bridge.c yogimpi.h
	$(MPICC) $(CFLAGS) -c yogimpi.c
	$(MPICC) $(CFLAGS) -c yogimpi_f90bridge.c
	$(MPICC) $(CFLAGS) $(LDFLAGS) yogimpi.o yogimpi_f90bridge.o \
              -o libyogimpi.so

clean:
	$(RM) *.o *.so nonblock sendrecv matt fsendrecv fmatt simple \
              writeFile1 datafile

test: default 
	$(CC) $(CFLAGS) test/simple.c -L. -lyogimpi -o simple
	$(CC) $(CFLAGS) test/nonBlocking.c -L. -lyogimpi -o nonblock
	$(CC) $(CFLAGS) test/sendrecv.c -L. -lyogimpi -o sendrecv
	$(CC) $(CFLAGS) test/matt.c -L. -lyogimpi -o matt 
	$(CC) $(CFLAGS) test/writeFile1.c -L. -lyogimpi -o writeFile1 
	#$(F90) $(FFLAGS) test/fsendrecv.f90 -L. -lyogimpi -o fsendrecv 
	#$(F90) $(FFLAGS) test/ftest_Matt.f90 -L. -lyogimpi -o ftestMatt

