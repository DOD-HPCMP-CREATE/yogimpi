include ../../Make.flags
 
.PHONY: default clean

default: _pyogimpi.so

_pyogimpi.so: pyogimpi_wrap.o 
	$(SERIALC) -shared pyogimpi_wrap.o $(PYTHON_LDFLAGS) -L../ -lyogimpi -o _pyogimpi.so 

pyogimpi_wrap.o: pyogimpi_wrap.c
	$(SERIALC) $(CFLAGS) $(PYTHON_INCLUDE) -c pyogimpi_wrap.c

pyogimpi_wrap.c: pyogimpi.i 
	swig -python $(PYTHON_INCLUDE) pyogimpi.i

clean:
	$(RM) _pyogimpi.so pyogimpi_wrap.o pyogimpi_wrap.c *.py
