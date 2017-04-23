#include ../Make.flags
 
.PHONY: wrap clean

wrap: generate_wrap.py wrap_objects.py WrapMPI.xml  
	python generate_wrap.py WrapMPI.xml 

clean:
	$(RM) mpitoyogi.h yogimpi.h yogimpi.cxx *.pyc *.o test_YogiManager

manager: wrap YogiManager.cxx YogiManager.h
	mpicxx -DYOGI_DEBUG -c -fPIC -I. YogiManager.cxx
	mpicxx -DYOGI_DEBUG -I. -fPIC test_YogiManager.cxx YogiManager.o \
               -o test_YogiManager

#clean:
#	$(RM) *.mod *.o libyogimpi.$(LIBEXTENSION)
