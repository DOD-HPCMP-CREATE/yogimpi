include Make.flags
 
.PHONY: default clean test

default:
	$(MAKE) -C src

install: default
	install -d -m 750 $(INSTALLDIR)
	install -d -m 750 $(INSTALLDIR)/bin
	install -d -m 750 $(INSTALLDIR)/lib
	install -d -m 750 $(INSTALLDIR)/include
	install -m 640 src/libyogimpi.so $(INSTALLDIR)/lib
	install -m 640 src/mpitoyogi.h $(INSTALLDIR)/include
	install -m 640 src/yogimpi.h $(INSTALLDIR)/include
	install -m 640 src/yogimpif.h $(INSTALLDIR)/include
	install -m 750 preprocess/yogicpp.py $(INSTALLDIR)/bin
	install -m 640 preprocess/AVUtility.py $(INSTALLDIR)/bin
	install -m 640 preprocess/preprocessor.xml $(INSTALLDIR)/bin

test: default 
	$(MAKE) -C test

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
