#include "mpi.h"
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int i, index;
    int bufferSize = 4;

    int sendBuffer[bufferSize];
    int receiveBuffer[bufferSize];
    for (i = 0; i < bufferSize; i++) {
      sendBuffer[i] = (i * 11) + 33;
    }

    MPI_Request request[4];
    MPI_Status status[4];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 4)
    {
        printf("Please run with 4 processes.\n");fflush(stdout);
        MPI_Finalize();
        return 1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        for (i=0; i < size-1; i++)
        {
            MPI_Isend(&sendBuffer[i+1], 1, MPI_INT, i+1, 123, MPI_COMM_WORLD, &request[i]);
        }
        for (i=0; i < size-1; i++)
        {
            MPI_Waitany(size-1, request, &index, status);
            assert(index != MPI_UNDEFINED);
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
