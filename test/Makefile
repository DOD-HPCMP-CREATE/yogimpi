include ../Make.flags

YCC=$(INSTALLDIR)/bin/mpicc
YF90=$(INSTALLDIR)/bin/mpif90
EXTRAFLAGS=-cpp

.PHONY: clean test ctest ftest

test: ctest ftest 

ctest: 
	$(YCC) $(CFLAGS) simple.c -o simple
	$(YCC) $(CFLAGS) nonBlocking.c -o nonblock
	$(YCC) $(CFLAGS) sendrecv.c -o sendrecv
	$(YCC) $(CFLAGS) matt.c -o matt 
	$(YCC) $(CFLAGS) writeFile1.c -o writeFile1 
	$(YCC) $(CFLAGS) nonblock_waitall.c -o nonblock_waitall
	$(YCC) $(CFLAGS) sendRecvErrC.c -o sendRecvErrC
	$(YCC) $(CFLAGS) testComms.c -o testComms
	$(YCC) $(CFLAGS) probe.c -o probe 
	$(YCC) $(CFLAGS) check_status.c -o check_status
	$(YCC) $(CFLAGS) collective.c -o collective 
	$(YCC) $(CFLAGS) waitsome.c -o waitsome
	$(YCC) $(CFLAGS) waitany.c -o waitany

ftest:
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) writeFile1.f90 -o fwriteFile1
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) f_gatherscatter.f90 -o f_gatherscatter
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) fsendrecv.f90 -o fsendrecv 
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) fmatt.f90 -o fmatt
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) fnonBlocking.f90 -o fnonblock
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) nonblock_waitall.f90 \
                -o fnonblock_waitall
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) testComms.f90 -o ftestComms
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) collective.f90 -o fcollective
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) waitsome.f90 -o fwaitsome

clean:
	$(RM) *.o nonblock sendrecv matt fsendrecv fmatt simple \
              writeFile1 datafile fwriteFile1 f_gatherscatter fnonblock \
              nonblock_waitall fnonblock_waitall sendRecvErrC testComms \
              ftestComms probe check_status collective fcollective \
              waitsome waitany fwaitsome
