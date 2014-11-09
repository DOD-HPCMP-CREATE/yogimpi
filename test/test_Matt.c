#include "yogimpi.h"
#include <stdio.h>

int main(int argc, char* argv[])

{

  int np, me, ier, flag=0, msglen=-1;

  YogiMPI_Request request;

  YogiMPI_Status status;

  int buf[1]; buf[0]=-1;


  ier = YogiMPI_Init(&argc, &argv);

  ier = YogiMPI_Comm_size(YogiMPI_COMM_WORLD, &np);

  ier = YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, &me);


  if(me > 1) printf("I am the useless processor #%d on %d\n", me, np);

  else printf("I am the working processor #%d on %d\n", me, np);


  ier = YogiMPI_Barrier(YogiMPI_COMM_WORLD);

  if(!me) {

    buf[0] = 69;

    ier = YogiMPI_Isend(buf, 1, YogiMPI_INT, 1,1, YogiMPI_COMM_WORLD, &request);

    ier = YogiMPI_Wait(&request, &status);

  }


  ier = YogiMPI_Barrier(YogiMPI_COMM_WORLD);


  if( me == 1 ) {

    ier = YogiMPI_Irecv(buf, 1, YogiMPI_INT, 0, 1, YogiMPI_COMM_WORLD, &request ) ;

    ier = YogiMPI_Wait(&request, &status );

    ier = YogiMPI_Get_count(&status, YogiMPI_INT, &msglen);


    if(msglen != 1) printf("ERROR: The length of the message is not 1\n");

    else printf("SUCCESS !\n");

  }

  ier = YogiMPI_Finalize();

}
