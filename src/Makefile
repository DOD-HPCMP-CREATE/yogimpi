#include ../Make.flags
 
.PHONY: wrap clean

wrap: generate_wrap.py wrap_objects.py WrapMPI.xml  
	python generate_wrap.py WrapMPI.xml 

clean:
	$(RM) mpitoyogi.h yogimpi.h yogimpi.cxx *.pyc *.o

#default: yogimpi.c yogimpi_f90bridge.c yogimpi.h mpitoyogi.h yogimpif.h yogimpi_fortran.f90
#	$(MPICXX) $(CFLAGS) -c yogimpi.cxx
#	$(F90) $(FFLAGS) -c yogimpi_fortran.f90
#	$(F90) $(FFLAGS) -c yogimpi_module.f90
#	$(CC) $(CFLAGS) -c yogimpi_f90bridge.c
#	$(MPICXX) $(CFLAGS) $(LDFLAGS) yogimpi.o yogimpi_module.o \
#                 yogimpi_f90bridge.o yogimpi_fortran.o -o \
#                 libyogimpi.$(LIBEXTENSION)

#clean:
#	$(RM) *.mod *.o libyogimpi.$(LIBEXTENSION)
