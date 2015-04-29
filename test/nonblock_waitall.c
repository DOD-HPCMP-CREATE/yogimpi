/*
A simple MPI example program using non-blocking send and receive
and MPI_Waitall to wait for completion of all message transfers.

The program consists of one processes (process 0) which sends a message
containing its identifier to N-1 other processes. These receive the
message and send it back. All processes use non-blocking send and receive
operations (MPI_Isend and MPI_Irecv, and MPI_Waitall to wait until the
messages have arrived).

Run the program with at least 2 processes.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "mpi.h"

int main(int argc, char* argv[]) {

  int MAXPROC = 4;
  int i, x, np, me;
  int tag = 42;
  int expectReduceSum = 6;
  int actualReduceSum = 0;

  MPI_Status status[MAXPROC];
  /* Request objects for non-blocking send and receive */
  MPI_Request send_req[MAXPROC], recv_req[MAXPROC];
  int y[MAXPROC];  /* Array to receive values in */
  for (i = 0; i < MAXPROC; i++) {
    y[i] = -1;
  }

  MPI_Init(&argc, &argv);                /* Initialize */
  MPI_Comm_size(MPI_COMM_WORLD, &np);    /* Get nr of processes */
  MPI_Comm_rank(MPI_COMM_WORLD, &me);    /* Get own identifier */

  if (np != MAXPROC) {
    if (me == 0) {
      printf("Please run with %d processes.\n", MAXPROC);
    }
    MPI_Finalize();
    exit(0);
  }
  
  x = me;   /* This is the value we send, the process id */

  if (me == 0) {    /* Process 0 does this */
    
    /* Send a message containing the process id to all other processes */
    for (i=1; i<np; i++) {
      MPI_Isend(&x, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &send_req[i]);
    }
    /* While the messages are delivered, we could do computations here */

    /* Wait until all messages have been sent */
    /* Note that we use requests and statuses starting from index 1 
       since process zero does not send to it self */ 
    MPI_Waitall(np-1, &send_req[1], &status[1]);

    /* Receive a message from all other processes */
    for (i=1; i<np; i++) {
      MPI_Irecv(&y[i], 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &recv_req[i]);
    }
    /* While the messages are delivered, we could do computations here */

    /* Wait until all messages have been received */
    /* Requests and statuses start from index 1 */ 
    MPI_Waitall(np-1, &recv_req[1], &status[1]);

    /* Print out one line for each message we received */
    for (i=1; i<np; i++) {
        actualReduceSum += y[i];
    }
    assert(actualReduceSum == expectReduceSum); 

  } else { /* all other processes do this */

    MPI_Irecv(&y, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &recv_req[0]);
    MPI_Wait(&recv_req[0], &status[0]);
    assert(y[0] == 0);

    MPI_Isend(&x, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &send_req[0]);
    /* We could do lots of computations here */
    MPI_Wait(&send_req[0], &status[0]);

  }

  MPI_Finalize();
  exit(0);
}
