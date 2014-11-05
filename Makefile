CC=mpicc
CFLAGS=-I. -fPIC
LDFLAGS=-shared

.PHONY: default clean

default: yogimpi.c yogimpi.h
	$(CC) $(CFLAGS) -c yogimpi.c
	$(CC) $(CFLAGS) $(LDFLAGS) yogimpi.o -o libyogimpi.so

clean:
	$(RM) *.o *.so
