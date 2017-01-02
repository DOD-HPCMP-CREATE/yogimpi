.PHONY: clean all

all: generate_wrap.py wrap_objects.py PresentMPI.xml SortedMissing.xml
	python generate_wrap.py PresentMPI.xml

clean:
	$(RM) *.h *.cxx *.pyc
