// Transmit data of different types using their associated datatype constants.

#include "mpi.h"
#include <assert.h>

void test_INT(MPI_Comm comm, int rank, int exp) {
    int send = rank;
    int recv = -1;
    MPI_Allreduce(&send, &recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    assert(recv == exp);
}

void test_LONG(MPI_Comm comm, int rank, int exp) {
    long int send = rank;
    long int recv = -1;
    MPI_Allreduce(&send, &recv, 1, MPI_LONG, MPI_SUM, MPI_COMM_WORLD);
    assert(recv == (long int)exp);
}

void test_FLOAT(MPI_Comm comm, int rank, int exp) {
    float send = (float)rank;
    float recv = -1.0;
    MPI_Allreduce(&send, &recv, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
    assert(recv == (float)exp);
}

void test_DOUBLE(MPI_Comm comm, int rank, int exp) {
    double send = (double)rank;
    double recv = -1.0;
    MPI_Allreduce(&send, &recv, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    assert(recv == (double)exp);
}

void test_COUNT(MPI_Comm comm, int rank, int exp) {
    MPI_Count send = (MPI_Count)rank;
    MPI_Count recv = -1.0;
    MPI_Allreduce(&send, &recv, 1, MPI_COUNT, MPI_SUM, MPI_COMM_WORLD);
    assert(recv == (MPI_Count)exp);
}

int main(int argc, char *argv[]) {
    int rank = -1;
    int size = -1;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int exp = 0;
    for (int i=0; i<size; ++i) {
        exp = exp + i;
    }

    test_INT(MPI_COMM_WORLD, rank, exp);
    test_LONG(MPI_COMM_WORLD, rank, exp);
    test_FLOAT(MPI_COMM_WORLD, rank, exp);
    test_DOUBLE(MPI_COMM_WORLD, rank, exp);
    test_COUNT(MPI_COMM_WORLD, rank, exp);

    MPI_Finalize();
    return 0;
}
