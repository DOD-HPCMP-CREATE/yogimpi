-include Make.version
-include Make.flags

# If YOGIMPI_VERSION is defined in Make.version, then it will be left alone.
# If not, then it will be set using "git describe --tags".
# If after all that it is empty, it will be replaced with "unknown".
YOGIMPI_VERSION ?= $(shell git describe --tags)
ifeq ($(YOGIMPI_VERSION),)
YOGIMPI_VERSION := unknown
endif

ARCHIVE_ROOT := yogimpi-$(YOGIMPI_VERSION)
ARCHIVE_FILE := $(ARCHIVE_ROOT).tar.gz


.PHONY: default clean check test distclean realclean

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
	install -m 640 Make.version $(INSTALLDIR)

test: default
	$(MAKE) -C test

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
	$(RM) -r __pycache__ *.pyc

distclean: clean
	$(RM) wrapper/mpicc
	$(RM) wrapper/mpifort
	$(RM) wrapper/mpicxx
	$(RM) wrapper/YogiMPIWrapper.py
	$(RM) test/testRunner.sh
	$(RM) etc/modulefile
	$(RM) etc/yogimpi.bashrc
	$(RM) etc/yogimpi.cshrc
	$(RM) Make.flags
	$(RM) Make.version
	$(RM) yogimpi-*.tar.gz

dist: distclean
	@echo "Creating Archive in $(ARCHIVE_FILE)"
	@echo "YOGIMPI_VERSION := $(YOGIMPI_VERSION)" > Make.version
	@echo "ARCHIVE_ROOT := $(ARCHIVE_ROOT)"
	tar --transform "s@^@$(ARCHIVE_ROOT)/@" -cf $(ARCHIVE_FILE) \
	    configure doc etc LICENSE Makefile Make.flags.in Make.version \
	    README.md src test wrapper

realclean: distclean
