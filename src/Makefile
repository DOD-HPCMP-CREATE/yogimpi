-include ../Make.version
-include ../Make.flags

.PHONY: wrap clean manager lib

lib: manager
	$(MPICXX) -c $(CXXFLAGS) $(DEBUGFLAGS) yogimpi.cxx
	$(MPICXX) -c $(CXXFLAGS) $(DEBUGFLAGS) yogimpi_f90bridge.cxx
	$(F90) -c $(FFLAGS) $(DEBUGFLAGS) yogimpi_module.f90
	$(F90) -c $(FFLAGS) $(DEBUGFLAGS) yogimpi_functions.f90
	$(MPICXX) $(LDFLAGS) $(CXXFLAGS) YogiManager.o yogimpi.o \
                  yogimpi_f90bridge.o yogimpi_module.o yogimpi_functions.o \
                  -ldl -o libyogimpi.so

manager: wrap YogiManager.cxx YogiManager.h
	$(MPICXX) -c $(CXXFLAGS) $(DEBUGFLAGS) YogiManager.cxx
	$(MPICXX) $(CXXFLAGS) $(DEBUGFLAGS) test_YogiManager.cxx YogiManager.o \
                  -ldl -o test_YogiManager

wrap: generate_wrap.py wrap_objects.py WrapMPI.xml
	$(PYTHON) generate_wrap.py --mpiver=$(MPIMAJVERSION).$(MPIMINVERSION) \
               --version="$(YOGIMPI_VERSION)" WrapMPI.xml

clean:
	$(RM) mpitoyogi.h yogimpi.h yogimpi.cxx yogimpif.h \
              *.pyc *.o *.so *.mod \
              test_YogiManager yogimpi_functions.f90 yogimpi_f90bridge.cxx
	$(RM) -r __pycache__
