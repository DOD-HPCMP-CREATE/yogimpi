#include "mpi.h"
#include <assert.h>
#include <stdio.h>

void addem(int *invec, int *inoutvec, int *len, MPI_Datatype *dtype)
{
    MPI_Datatype rtype = Yogi_ResolveDatatype(dtype);
    assert(rtype == MPI_INT);
    int i;
    for ( i=0; i<*len; i++ )
        inoutvec[i] += invec[i];
}

int main( int argc, char **argv )
{
    int rank, size, i;
    int data;
    int errors=0;
    int result = -100;
    int correct_result;
    MPI_Op op;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    data = rank;
    MPI_Op_create( (MPI_User_function *)addem, 1, &op );
    MPI_Reduce ( &data, &result, 1, MPI_INT, op, 0, MPI_COMM_WORLD );
    MPI_Bcast ( &result, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Op_free( &op );
    correct_result = 0;
    for(i=0;i<size;i++)
        correct_result += i;
    if (result != correct_result) errors++;

    if(errors) printf("FAIL - Errors occurred.\n");

    MPI_Finalize();
    return errors;
}
