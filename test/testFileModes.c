#include "mpi.h"
#include <stdio.h>

int main(int argc, char **argv){

    int rank, size, inMode, outMode;
    int testStatus = 0;
    MPI_File fhw;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    inMode = MPI_MODE_CREATE|MPI_MODE_WRONLY|MPI_MODE_DELETE_ON_CLOSE;
    MPI_File_open(MPI_COMM_WORLD, "exampleFile", inMode, MPI_INFO_NULL, &fhw);
    MPI_File_get_amode(fhw, &outMode);
    if (outMode != inMode) { 
        printf("test error, value is %d, expected %d\n", outMode, inMode);
        testStatus = 1; 
    }
    
    MPI_File_close(&fhw);
    MPI_Finalize();
    return testStatus;
}

