#include "mpitoyogi.h"
#include <stdio.h>

int main(int argc, char* argv[])

{

  int np, me, ier, flag=0, msglen=-1;

  MPI_Request request;

  MPI_Status status;

  int buf[1]; buf[0]=-1;


  ier = MPI_Init(&argc, &argv);

  ier = MPI_Comm_size(MPI_COMM_WORLD, &np);

  ier = MPI_Comm_rank(MPI_COMM_WORLD, &me);


  if(me > 1) printf("I am the useless processor #%d on %d\n", me, np);

  else printf("I am the working processor #%d on %d\n", me, np);


  ier = MPI_Barrier(MPI_COMM_WORLD);

  if(!me) {

    buf[0] = 69;

    ier = MPI_Isend(buf, 1, MPI_INT, 1,1, MPI_COMM_WORLD, &request);

    ier = MPI_Wait(&request, &status);

  }


  ier = MPI_Barrier(MPI_COMM_WORLD);


  if( me == 1 ) {

    ier = MPI_Irecv(buf, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &request ) ;

    ier = MPI_Wait(&request, &status );

    ier = MPI_Get_count(&status, MPI_INT, &msglen);


    if(msglen != 1) printf("ERROR: The length of the message is not 1\n");

    else printf("SUCCESS !\n");

  }

  ier = MPI_Finalize();

}
