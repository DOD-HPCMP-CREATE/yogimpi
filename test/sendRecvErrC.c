/**
   @file      sendRecvErrC.c
   @author    Mitch Richling <http://www.mitchr.me/>
   @Copyright Copyright 1999 by Mitch Richling.  All rights reserved.
   @brief     Demo MPI functions MPI_Send and MPI_Recv@EOL
   @Keywords  LAM MPI
   @Std       C99 MPI1

              This program demonstrates the following functions:

                    MPI_Send
                    MPI_Recv
                    MPI_Abort

              This program demonstrates a way to avoid a typical
              deadlock situation encountered in many MPI applications.
              The idea is to use MPI_Iprobe to check if a message is
              waiting before we call a blocking MPI_Recv function.  We
              keep probing until we have a message or we have waited
              too long.
*/

#include <stdio.h>
#include "mpitoyogi.h"

int main(int argc, char *argv[]) {
  int rank, size, n, rslt, i, flags;
  MPI_Status status;
  int needData, msgProbeCount=0;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(rank == 0) {
    printf("Mr. Zero has come on line....\n");
    printf("Mr. Zero waiting for %d slaves to check in.\n", size);
    for(i=1;i<size;i++) {
      
      /* Many production MPI programs are quite badly behaved in the
         face of unexpected failures.  For example, it would be
         typical to see the following loop using Iprobe simply missing
         from most production codes.  Such code would hang forever if
         one of the slaves (nonzero rank processes) were to
         prematurely crash.  With just a little bit of care and code,
         we can avoid the vast majority of such cases.  The following
         loop, for example, still leaves a chance open for deadlock
         but it will catch the vast majority of slave crash events
         possible.  In order to illustrate the idea behind this loop I
         have removed the checks for the function return codes.*/
      needData=1;
      while(needData) {
        msgProbeCount++;
        if(msgProbeCount > 100) {
          printf("ERROR: Timeout -- not all expected messages received.\n");
          MPI_Abort(MPI_COMM_WORLD, 1);
        } /* end if */
        /* Check for a waiting message */
        rslt = MPI_Iprobe(MPI_ANY_SOURCE,  // Who to receive data from
                          MPI_ANY_TAG,     // Tags to receive        
                          MPI_COMM_WORLD,  // Group from which to receive
                          &flags,          // 1 if we have a msg, 0 otherwise
                          &status);        // Same stuff we get from a call to MPI_Recv
        if(flags == 1) {
            printf("Message Ready Probe Return: (%d,%d,%d,%d)\n", 
                   status.MPI_SOURCE, status.MPI_TAG, status.MPI_ERROR, rslt);
            needData=0;
        } else {
          printf("No message ready right now\n");
          sleep(2);
        } /* end if/else */
      } /* end while */

      /* Receive a message. */
      rslt = MPI_Recv(&n, 1, MPI_INT, // Location of data, number of elements, type
                      MPI_ANY_SOURCE, // Who to receive data from
                      MPI_ANY_TAG,    // Tags to receive
                      MPI_COMM_WORLD, // Group from which to receive
                      &status);       // Status of call
      printf("Process %d has checked in (%d,%d,%d,%d)\n", 
             n, status.MPI_SOURCE, status.MPI_TAG, status.MPI_ERROR, rslt);
    } /* end for */
  } else {
    /* Sleep a bit to help demonstrate the probe and wait code in the
       rank 0 process. */
    sleep(rank/2+1);

    /* Send a message. MPI_Ssend(...) is the same thing, but it blocks
       until the receiver starts to read data. */
    MPI_Send(&rank, 1, MPI_INT, // The data, number of elements, and type
             0,                 // Who to send data too
             999,               // Tag to use (random number in this case)
             MPI_COMM_WORLD);   // The group we are sending to.
  }

  MPI_Finalize();
  return 0;
}
