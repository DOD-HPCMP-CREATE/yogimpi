#include "mpi.h"
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    int rank, size, i;
    int buffer[10];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (size < 2)
    {
        printf("Please run with two processes.\n");fflush(stdout);
        MPI_Finalize();
        return 0;
    }
    if (rank == 0)
    {
        for (i=0; i<10; i++)
            buffer[i] = i;
        MPI_Send(buffer, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
    }
    if (rank == 1)
    {
        for (i=0; i<10; i++)
            buffer[i] = -1;
        MPI_Recv(buffer, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, &status);
        for (i=0; i<10; i++)
        {
            assert(buffer[i] == i);
        }
    }
    MPI_Finalize();
    return 0;
}
