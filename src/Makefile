include ../Make.flags
 
.PHONY: default clean

default: yogimpi.c yogimpi_f90bridge.c yogimpi.h
	$(MPICC) $(CFLAGS) -c yogimpi.c
	$(MPICC) $(CFLAGS) -c yogimpi_f90bridge.c
	$(MPICC) $(CFLAGS) $(LDFLAGS) yogimpi.o yogimpi_f90bridge.o \
                 -o libyogimpi.so

clean:
	$(RM) yogimpi.o yogimpi_f90bridge.o libyogimpi.so
