include ../Make.flags

LINKYOGI=-L -lyogimpi

.PHONY: clean test ctest ftest

test: ctest ftest 

ctest: default
	$(CC) $(CFLAGS) test/simple.c $(LINKYOGI) -o simple
	$(CC) $(CFLAGS) test/nonBlocking.c $(LINKYOGI) -o nonblock
	$(CC) $(CFLAGS) test/sendrecv.c $(LINKYOGI) -o sendrecv
	$(CC) $(CFLAGS) test/matt.c $(LINKYOGI) -o matt 
	$(CC) $(CFLAGS) test/writeFile1.c -L. -lyogimpi -o writeFile1 
	$(CC) $(CFLAGS) test/nonblock_waitall.c -L. -lyogimpi \
              -o nonblock_waitall
	$(CC) $(CFLAGS) test/sendRecvErrC.c -L. -lyogimpi -o sendRecvErrC
	$(CC) $(CFLAGS) test/testComms.c -L. -lyogimpi -o testComms
	$(CC) $(CFLAGS) test/probe.c -L. -lyogimpi -o probe 
	$(CC) $(CFLAGS) test/check_status.c -L. -lyogimpi -o check_status
	$(CC) $(CFLAGS) test/collective.c -L. -lyogimpi -o collective 
	$(CC) $(CFLAGS) test/waitsome.c -L. -lyogimpi -o waitsome
	$(CC) $(CFLAGS) test/waitany.c -L. -lyogimpi -o waitany

ftest:
	$(F90) $(FFLAGS) test/writeFile1.f90 -L. -lyogimpi -o fwriteFile1
	$(F90) $(FFLAGS) test/f_gatherscatter.f90 -L. -lyogimpi \
               -o f_gatherscatter
	$(F90) $(FFLAGS) test/fsendrecv.f90 -L. -lyogimpi -o fsendrecv 
	$(F90) $(FFLAGS) test/fmatt.f90 -L. -lyogimpi -o fmatt
	$(F90) $(FFLAGS) test/fnonBlocking.f90 -L. -lyogimpi -o fnonblock
	$(F90) $(FFLAGS) test/nonblock_waitall.f90 -L. -lyogimpi \
               -o fnonblock_waitall
	$(F90) $(FFLAGS) test/testComms.f90 -L. -lyogimpi -o ftestComms
	$(F90) $(FFLAGS) test/collective.f90 -L. -lyogimpi -o fcollective
	$(F90) $(FFLAGS) test/waitsome.f90 -L. -lyogimpi -o fwaitsome

clean:
	$(RM) *.o *.so nonblock sendrecv matt fsendrecv fmatt simple \
              writeFile1 datafile fwriteFile1 f_gatherscatter fnonblock \
              nonblock_waitall fnonblock_waitall sendRecvErrC testComms \
              ftestComms probe check_status collective fcollective \
              waitsome waitany fwaitsome
