include Make.flags
 
.PHONY: default clean test realclean python

default: src/libyogimpi.$(LIBEXTENSION)
ifeq ($(ENABLEPYTHON),yes)
	$(MAKE) -C src/python
endif

src/libyogimpi.$(LIBEXTENSION):
	$(MAKE) -C src

install: default
	install -d -m 750 $(INSTALLDIR)
	install -d -m 750 $(INSTALLDIR)/bin
	install -d -m 750 $(INSTALLDIR)/lib
	install -d -m 750 $(INSTALLDIR)/include
	install -d -m 750 $(INSTALLDIR)/etc
	install -m 640 src/libyogimpi.$(LIBEXTENSION) $(INSTALLDIR)/lib
	install -m 640 src/mpitoyogi.h $(INSTALLDIR)/include/mpi.h
	install -m 640 src/yogimpi.h $(INSTALLDIR)/include
	install -m 640 src/yogimpif.h $(INSTALLDIR)/include/mpif.h
	install -m 640 src/yogiconvutil.h $(INSTALLDIR)/include
	install -m 640 preprocess/YogiSupported.xml $(INSTALLDIR)/bin
	install -m 640 preprocess/AVUtility.py $(INSTALLDIR)/bin
	install -m 640 etc/yogimpi.bashrc $(INSTALLDIR)/etc
	install -m 640 etc/yogimpi.cshrc $(INSTALLDIR)/etc
	install -m 640 etc/modulefile $(INSTALLDIR)/etc
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
ifeq ($(ENABLEPYTHON),yes)
	$(MAKE) -C src/python clean
endif

realclean: clean
	$(RM) wrapper/mpicc
	$(RM) wrapper/mpif90
	$(RM) wrapper/mpif77
	$(RM) wrapper/mpicxx
	$(RM) test/testRunner.sh
	$(RM) etc/modulefile
	$(RM) etc/yogimpi.bashrc
	$(RM) etc/yogimpi.cshrc
ifeq ($(ENABLEPYTHON),yes)
	$(MAKE) -C src/python clean
endif
	$(RM) Make.flags
