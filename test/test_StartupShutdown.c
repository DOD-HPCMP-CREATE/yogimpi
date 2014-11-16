#include <stdio.h>
#include "mpitoyogi.h"

int main() {
    MPI_Init(NULL, NULL);
    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("Hello, world, I am on rank %d\n", rank);
    MPI_Finalize();
    return 0;
}
