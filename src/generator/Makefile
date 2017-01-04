.PHONY: clean all

all: generate_wrap.py wrap_objects.py WrapMPI.xml 
	python generate_wrap.py WrapMPI.xml 

clean:
	$(RM) *.h *.cxx *.pyc
