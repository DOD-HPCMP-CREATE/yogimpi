include Make.flags
 
.PHONY: default clean test realclean python

default: src/libyogimpi.so

src/libyogimpi.so:
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
	install -m 750 wrapper/mpif77 $(INSTALLDIR)/bin
	install -m 640 Make.flags $(INSTALLDIR)

ifeq ($(ENABLEPYTHON),yes)
	install -d -m 750 $(INSTALLDIR)/python
	install -m 640 src/python/_pyogimpi.so $(INSTALLDIR)/python
endif


test: default 
	$(MAKE) -C test

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean

realclean: clean
	$(RM) wrapper/mpicc
	$(RM) wrapper/mpif90
	$(RM) wrapper/mpicxx
	$(MAKE) python_clean
	$(RM) Make.flags

# Normally these are not required for end-users to run.  Mainly provided for
# buildmasters and developers.
python: default
ifeq ($(ENABLEPYTHON),yes)
	$(MAKE) -C src/python
else
	@echo "Python support was not enabled by configure script."
endif

python_clean:
	$(MAKE) -C src/python clean
