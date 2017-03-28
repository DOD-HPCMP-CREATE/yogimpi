.PHONY: clean all

all: generate_wrap.py wrap_objects.py WrapMPI.xml  
	python generate_wrap.py WrapMPI.xml 

clean:
	$(RM) mpitoyogi.h yogimpi.h yogimpi.cxx *.pyc
