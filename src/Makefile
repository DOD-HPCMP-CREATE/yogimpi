include ../Make.flags
 
.PHONY: wrap clean manager lib

lib: manager
	$(MPICXX) -c $(CXXFLAGS) yogimpi.cxx
	$(MPICXX) -c $(CXXFLAGS) yogimpi_f90bridge.cxx
	$(F90) -c $(FFLAGS) yogimpi_module.f90
	$(F90) -c $(FFLAGS) yogimpi_functions.f90
	$(MPICXX) $(LDFLAGS) $(CXXFLAGS) YogiManager.o yogimpi.o \
                  yogimpi_f90bridge.o yogimpi_module.o yogimpi_functions.o \
                  -ldl -o libyogimpi.so

manager: wrap YogiManager.cxx YogiManager.h
	$(MPICXX) -c $(CXXFLAGS) YogiManager.cxx
	$(MPICXX) $(CXXFLAGS) test_YogiManager.cxx YogiManager.o \
                  -ldl -o test_YogiManager

wrap: generate_wrap.py wrap_objects.py WrapMPI.xml  
	python generate_wrap.py --mpi=$(MPIVERSION) WrapMPI.xml 

clean:
	$(RM) mpitoyogi.h yogimpi.h yogimpi.cxx *.pyc *.o *.so *.mod \
              test_YogiManager yogimpi_functions.f90 yogimpi_f90bridge.cxx
