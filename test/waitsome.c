#include "mpi.h"
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    const int RUN_WITH = 4;
    int rank, size;
    int i, index[RUN_WITH], count, remaining;
    int sendBuffer[RUN_WITH];
    int receiveBuffer[RUN_WITH];
    MPI_Request request[RUN_WITH];
    MPI_Status status[RUN_WITH];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != RUN_WITH) {
        printf("Please run with %d processes.\n", RUN_WITH);
        fflush(stdout);
        MPI_Finalize();
        return 1;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (i=0; i < size; i++) {
        sendBuffer[i] = (i * 11) + 42;
    }

    if (rank == 0)
    {
        for (i=0; i<size-1; i++)
        {
            MPI_Isend(&sendBuffer[i+1], 1, MPI_INT, i+1, 123, MPI_COMM_WORLD, &request[i]);
        }
        remaining = size-1;
        while (remaining > 0)
        {
            MPI_Waitsome(size-1, request, &count, index, status);
            assert(count != MPI_UNDEFINED);
            if (count > 0)
            {
                remaining = remaining - count;
            }
        }
    }
    else
    {
        MPI_Recv(receiveBuffer, 1, MPI_INT, 0, 123, MPI_COMM_WORLD, &status[0]);
        assert(receiveBuffer[0] == sendBuffer[rank]);
    }

    MPI_Finalize();
    return 0;
}
