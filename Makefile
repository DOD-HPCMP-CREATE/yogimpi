include Make.flags
 
.PHONY: default clean test realclean

default:
	$(MAKE) -C src

install: default
	install -d -m 750 $(INSTALLDIR)
	install -d -m 750 $(INSTALLDIR)/bin
	install -d -m 750 $(INSTALLDIR)/lib
	install -d -m 750 $(INSTALLDIR)/include
	install -m 640 src/libyogimpi.so $(INSTALLDIR)/lib
	install -m 640 src/mpitoyogi.h $(INSTALLDIR)/include/mpi.h
	install -m 640 src/yogimpi.h $(INSTALLDIR)/include
	install -m 640 src/yogimpif.h $(INSTALLDIR)/include/mpif.h
	install -m 640 preprocess/preprocessor.xml $(INSTALLDIR)/bin
	install -m 640 preprocess/AVUtility.py $(INSTALLDIR)/bin
	install -m 750 wrapper/mpicc $(INSTALLDIR)/bin
	install -m 750 wrapper/mpicxx $(INSTALLDIR)/bin
	install -m 750 wrapper/mpif90 $(INSTALLDIR)/bin

test: default 
	$(MAKE) -C test

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean

realclean: clean
	$(RM) wrapper/mpicc
	$(RM) wrapper/mpif90
	$(RM) wrapper/mpicxx
	$(RM) Make.flags
