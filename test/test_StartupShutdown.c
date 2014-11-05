#include <stdio.h>
#include "yogimpi.h"

int main() {
    YogiMPI_Init(NULL, NULL);
    int rank = -1;
    YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, &rank);
    printf("Hello, world, I am on rank %d\n", rank);
    YogiMPI_Finalize();
    return 0;
}
