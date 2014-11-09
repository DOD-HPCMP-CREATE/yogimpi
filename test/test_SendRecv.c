#include "yogimpi.h"
#include <stdio.h>
 
int main(int argc, char *argv[])
{
    int rank, size, i;
    int buffer[10];
    YogiMPI_Status status;
 
    YogiMPI_Init(&argc, &argv);
    YogiMPI_Comm_size(YogiMPI_COMM_WORLD, &size);
    YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, &rank);
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
        YogiMPI_Send(buffer, 10, YogiMPI_INT, 1, 123, YogiMPI_COMM_WORLD);
    }
    if (rank == 1)
    {
        for (i=0; i<10; i++)
            buffer[i] = -1;
        YogiMPI_Recv(buffer, 10, YogiMPI_INT, 0, 123, YogiMPI_COMM_WORLD, &status);
        for (i=0; i<10; i++)
        {
            if (buffer[i] != i)
                printf("Error: buffer[%d] = %d but is expected to be %d\n", i, buffer[i], i);
        }
        fflush(stdout);
    }
    YogiMPI_Finalize();
    return 0;
}
