include ../Make.flags
 
.PHONY: wrap clean manager lib

lib: manager
	mpicxx -c $(CXXFLAGS) yogimpi.cxx
	mpicxx -c $(CXXFLAGS) yogimpi_f90bridge.cxx
	$(F90) -c $(FFLAGS) yogimpi_module.f90
	$(F90) -c $(FFLAGS) yogimpi_functions.f90
	mpicxx $(LDFLAGS) $(CXXFLAGS) YogiManager.o yogimpi.o \
               yogimpi_f90bridge.o yogimpi_module.o yogimpi_functions.o \
               -o libyogimpi.so

manager: wrap YogiManager.cxx YogiManager.h
	mpicxx -c $(CXXFLAGS) YogiManager.cxx
	mpicxx $(CXXFLAGS) test_YogiManager.cxx YogiManager.o \
               -o test_YogiManager

wrap: generate_wrap.py wrap_objects.py WrapMPI.xml  
	python generate_wrap.py WrapMPI.xml 

clean:
	$(RM) mpitoyogi.h yogimpi.h yogimpi.cxx *.pyc *.o *.so *.mod \
              test_YogiManager yogimpi_functions.f90 yogimpi_f90bridge.cxx
