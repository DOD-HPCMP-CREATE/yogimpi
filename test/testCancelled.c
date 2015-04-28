#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
/* Test of various send cancel calls */
int main( int argc, char *argv[] )
{
    int errs = 0;
    int rank, size, source, dest;
    MPI_Comm comm;
    MPI_Status status;
    MPI_Request req;
    static int bufsizes[4] = { 1, 100, 10000, 1000000 };
    char *buf;
    int cs, flag, n;

    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );
    source = 0;
    dest = size - 1;

    for (cs=0; cs<4; cs++) {
        if (rank == 0) {
            n = bufsizes[cs];
            buf = (char *)malloc( n );
            if (!buf) {
                MPI_Abort( MPI_COMM_WORLD, 1 );
            }
            MPI_Isend( buf, n, MPI_CHAR, dest, cs+n+1, comm, &req );
            MPI_Cancel( &req );
            MPI_Wait( &req, &status );
            MPI_Test_cancelled( &status, &flag );
            if (!flag) {
                errs++;
            }
            else
            {
                n = 0;
            }
            /* Send the size, zero for successfully cancelled */
            MPI_Send( &n, 1, MPI_INT, dest, 123, comm );
            /* Send the tag so the message can be received */
            n = cs+n+1;
            MPI_Send( &n, 1, MPI_INT, dest, 123, comm );
            free( buf );
        }
        else if (rank == dest)
        {
            int n, tag;
            char *btemp;
            MPI_Recv( &n, 1, MPI_INT, 0, 123, comm, &status );
            MPI_Recv( &tag, 1, MPI_INT, 0, 123, comm, &status );
            if (n > 0)
            {
                /* If the message was not cancelled, receive it here */
                btemp = (char*)malloc( n );
                if (!btemp)
                {
                    MPI_Abort( MPI_COMM_WORLD, 1 );
                }
                MPI_Recv( btemp, n, MPI_CHAR, 0, tag, comm, &status );
                free(btemp);
            }
        }
        MPI_Barrier( comm );
    }

    MPI_Finalize();
    return errs;
}
