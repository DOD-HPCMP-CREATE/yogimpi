include ../Make.flags
 
.PHONY: default clean

default: yogimpi.c yogimpi_f90bridge.c yogimpi.h mpitoyogi.h yogimpif.h mpiftoyogi.h yogimpi_fortran.f90
	$(MPICC) $(CFLAGS) -c yogimpi.c
	$(F90) $(FFLAGS) -c yogimpi_fortran.f90
	$(F90) $(FFLAGS) -c yogimpi_module.f90
	$(CC) $(CFLAGS) -c yogimpi_f90bridge.c
	$(MPICC) $(CFLAGS) $(LDFLAGS) yogimpi.o yogimpi_module.o \
                 yogimpi_f90bridge.o yogimpi_fortran.o -o \
                 libyogimpi.$(LIBEXTENSION)

clean:
	$(RM) *.mod *.o libyogimpi.$(LIBEXTENSION)
