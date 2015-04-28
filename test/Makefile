include ../Make.flags

YCC=$(INSTALLDIR)/bin/mpicc
YF90=$(INSTALLDIR)/bin/mpif90

.PHONY: clean test ctest ftest

test: ctest ftest 

testCancelled: testCancelled.c
	$(YCC) $(CFLAGS) testCancelled.c -o testCancelled

errorHandler: errorHandler.c
	$(YCC) $(CFLAGS) errorHandler.c -o errorHandler

createOp: createOp.c
	$(YCC) $(CFLAGS) createOp.c -o createOp 

simple: simple.c
	$(YCC) $(CFLAGS) simple.c -o simple

nonBlocking: nonBlocking.c
	$(YCC) $(CFLAGS) nonBlocking.c -o nonBlocking

probe: probe.c
	$(YCC) $(CFLAGS) probe.c -o probe 

testAll: testAll.c
	$(YCC) $(CFLAGS) testAll.c -o testAll 

runtest: createOp errorHandler simple nonBlocking probe testAll testCancelled 
	mpirun -np 4 ./createOp
	mpirun -np 4 ./errorHandler
	mpirun -np 4 ./simple
	mpirun -np 2 ./nonBlocking
	mpirun -np 2 ./probe	
	mpirun -np 4 ./testAll
	mpirun -np 4 ./testCancelled

ctest: errorHandler createOp simple nonBlocking probe testAll testCancelled 
	$(YCC) $(CFLAGS) sendrecv.c -o sendrecv
	$(YCC) $(CFLAGS) writeFile1.c -o writeFile1 
	$(YCC) $(CFLAGS) nonblock_waitall.c -o nonblock_waitall
	$(YCC) $(CFLAGS) sendRecvErrC.c -o sendRecvErrC
	$(YCC) $(CFLAGS) testComms.c -o testComms
	$(YCC) $(CFLAGS) check_status.c -o check_status
	$(YCC) $(CFLAGS) collective.c -o collective 
	$(YCC) $(CFLAGS) waitsome.c -o waitsome
	$(YCC) $(CFLAGS) waitany.c -o waitany

ftest:
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) writeFile1.f90 -o fwriteFile1
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) f_gatherscatter.f90 -o f_gatherscatter
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) fsendrecv.f90 -o fsendrecv 
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) fnonBlocking.f90 -o fnonblock
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) nonblock_waitall.f90 \
                -o fnonblock_waitall
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) testComms.f90 -o ftestComms
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) collective.f90 -o fcollective
	$(YF90) $(FFLAGS) $(EXTRAFLAGS) waitsome.f90 -o fwaitsome

clean:
	$(RM) *.o nonBlocking sendrecv fsendrecv simple testCancelled \
              writeFile1 datafile fwriteFile1 f_gatherscatter fnonblock \
              nonblock_waitall fnonblock_waitall sendRecvErrC testComms \
              ftestComms probe check_status collective fcollective \
              waitsome waitany fwaitsome createOp errorHandler testAll
