include Make.flags

.PHONY: default clean test realclean

default: src/libyogimpi.$(LIBEXTENSION)

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
	install -m 640 src/yogimpi.mod $(INSTALLDIR)/include/yogimpi.mod
	install -m 640 etc/yogimpi.bashrc $(INSTALLDIR)/etc
	install -m 640 etc/yogimpi.cshrc $(INSTALLDIR)/etc
	install -m 640 etc/modulefile $(INSTALLDIR)/etc
	install -m 750 wrapper/mpicc $(INSTALLDIR)/bin
	install -m 750 wrapper/mpicxx $(INSTALLDIR)/bin
	install -m 750 wrapper/mpifort $(INSTALLDIR)/bin
	ln -srf $(INSTALLDIR)/bin/mpicxx $(INSTALLDIR)/bin/mpic++
	ln -srf $(INSTALLDIR)/bin/mpicxx $(INSTALLDIR)/bin/mpiCC
	ln -srf $(INSTALLDIR)/bin/mpifort $(INSTALLDIR)/bin/mpif77
	ln -srf $(INSTALLDIR)/bin/mpifort $(INSTALLDIR)/bin/mpif90
	install -m 750 wrapper/YogiMPIWrapper.py $(INSTALLDIR)/bin
	install -m 640 Make.flags $(INSTALLDIR)

test: default
	$(MAKE) -C test

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean

realclean: clean
	$(RM) wrapper/mpicc
	$(RM) wrapper/mpifort
	$(RM) wrapper/mpicxx
	$(RM) wrapper/YogiMPIWrapper.py
	$(RM) test/testRunner.sh
	$(RM) etc/modulefile
	$(RM) etc/yogimpi.bashrc
	$(RM) etc/yogimpi.cshrc
	$(RM) Make.flags
