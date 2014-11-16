#include <stdio.h>
#include "mpitoyogi.h"

int main(int argc, char **argv){

    int i, rank, size, offset, nints, N=16;
    MPI_File fhw;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int buf[N];
    for ( i=0;i<N;i++){
        buf[i] = i ;
    }
    offset = rank*(N/size)*sizeof(int);
    MPI_File_open(MPI_COMM_WORLD, "datafile", MPI_MODE_CREATE|MPI_MODE_WRONLY,
                  MPI_INFO_NULL, &fhw);
    printf("\nRank: %d, Offset: %d\n", rank, offset);
    MPI_File_write_at(fhw, offset, buf, (N/size), MPI_INT, &status);
    MPI_File_close(&fhw);
    MPI_Finalize();
    return 0;
}

