-include ../Make.version
-include ../Make.flags

YCC=$(INSTALLDIR)/bin/mpicc
YF90=$(INSTALLDIR)/bin/mpif90

.PHONY: clean test runctests runc2tests runc3tests runftests ctests ftests \
        c2tests c3tests

runtest: runctests runftests

ifeq ($(MPIMAJVERSION), 3)
runctests: runc2tests runc3tests
else
runctests: runc2tests
endif

test: ctests ftests

ifeq ($(MPIMAJVERSION), 3)
ctests: c2tests c3tests
else
ctests: c2tests
endif

c2tests: simple createOp errorHandler nonBlocking probe testAll writeFile1 \
         waitany collective sendrecv testComms nonblock_waitall waitsome \
         testAttr testInfo testFileModes types

c3tests: mprobe

testFileModes: testFileModes.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) testFileModes.c -o testFileModes

testInfo: testInfo.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) testInfo.c -o testInfo

testCancelled: testCancelled.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) testCancelled.c -o testCancelled

errorHandler: errorHandler.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) errorHandler.c -o errorHandler

createOp: createOp.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) createOp.c -o createOp

simple: simple.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) simple.c -o simple

nonBlocking: nonBlocking.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) nonBlocking.c -o nonBlocking

probe: probe.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) probe.c -o probe

mprobe: mprobe.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) mprobe.c -o mprobe

testAll: testAll.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) testAll.c -o testAll

writeFile1: writeFile1.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) writeFile1.c -o writeFile1

waitany: waitany.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) waitany.c -o waitany

collective: collective.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) collective.c -o collective

sendrecv: sendrecv.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) sendrecv.c -o sendrecv

testComms: testComms.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) testComms.c -o testComms

nonblock_waitall: nonblock_waitall.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) nonblock_waitall.c -o nonblock_waitall

waitsome: waitsome.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) waitsome.c -o waitsome

testAttr: testAttr.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) testAttr.c -o testAttr

types: types.c
	$(YCC) $(CFLAGS) $(DEBUGFLAGS) types.c -o types

runc3tests: c3tests
	./testRunner.sh 2 ./mprobe

runc2tests: c2tests
	./testRunner.sh 4 ./createOp
	./testRunner.sh 4 ./errorHandler
	./testRunner.sh 4 ./simple
	./testRunner.sh 2 ./nonBlocking
	./testRunner.sh 2 ./probe
	./testRunner.sh 4 ./testAll
	./testRunner.sh 4 ./writeFile1
	cmp cWriteFile.expected cWriteFile.result || exit 1
	./testRunner.sh 4 ./waitany
	./testRunner.sh 8 ./collective
	./testRunner.sh 2 ./sendrecv
	./testRunner.sh 4 ./testComms
	./testRunner.sh 4 ./nonblock_waitall
	./testRunner.sh 4 ./waitsome
	./testRunner.sh 4 ./testAttr
	./testRunner.sh 4 ./testInfo
	./testRunner.sh 2 ./testFileModes
	./testRunner.sh 4 ./types

runftests: ftests
	./testRunner.sh 2 ./fsimple
	./testRunner.sh 4 ./fwriteFile1
	cmp fWriteFile.expected fWriteFile.result || exit 1
	./testRunner.sh 2 ./fsendrecv
	./testRunner.sh 8 ./fcollective
	./testRunner.sh 4 ./ftestComms
	./testRunner.sh 4 ./f_gatherscatter
	./testRunner.sh 2 ./fnonblock
	./testRunner.sh 4 ./fwaitsome
	./testRunner.sh 2 ./fwtick
	./testRunner.sh 4 ./testInfo

fsimple: fsimple.f90
	$(YF90) $(FFLAGS) $(DEBUGFLAGS) fsimple.f90 -o fsimple

fwriteFile1: writeFile1.f90
	$(YF90) $(FFLAGS) $(DEBUGFLAGS) writeFile1.f90 -o fwriteFile1

fsendrecv: fsendrecv.f90
	$(YF90) $(FFLAGS) $(DEBUGFLAGS) fsendrecv.f90 -o fsendrecv

fcollective: collective.f90
	$(YF90) $(FFLAGS) $(DEBUGFLAGS) collective.f90 -o fcollective

ftestComms: testComms.f90
	$(YF90) $(FFLAGS) $(DEBUGFLAGS) testComms.f90 -o ftestComms

f_gatherscatter: f_gatherscatter.f90
	$(YF90) $(FFLAGS) $(DEBUGFLAGS) f_gatherscatter.f90 -o f_gatherscatter

fnonblock: fnonBlocking.f90
	$(YF90) $(FFLAGS) $(DEBUGFLAGS) fnonBlocking.f90 -o fnonblock

fwaitsome: waitsome.f90
	$(YF90) $(FFLAGS) $(DEBUGFLAGS) waitsome.f90 -o fwaitsome

fwtick: fwtick.f90
	$(YF90) $(FFLAGS) $(DEBUGFLAGS) fwtick.f90 -o fwtick

ftestInfo: testInfo.f90
	$(YF90) $(FFLAGS) $(DEBUGFLAGS) testInfo.f90 -o ftestInfo

ftests: fwriteFile1 fsendrecv fcollective ftestComms f_gatherscatter \
        fnonblock fwaitsome fsimple fwtick ftestInfo

clean:
	$(RM) *.o nonBlocking sendrecv fsendrecv simple testCancelled \
              writeFile1 fwriteFile1 f_gatherscatter fnonblock \
              nonblock_waitall testComms fsimple testInfo types \
              ftestComms probe mprobe collective fcollective fwtick \
              waitsome waitany fwaitsome createOp errorHandler testAll \
              cWriteFile.result fWriteFile.result testAttr ftestInfo \
              testFileModes yogimpi.log.*
	$(RM) -r __pycache__ *.pyc
