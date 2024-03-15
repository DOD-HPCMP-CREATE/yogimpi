#include "mpi.h"
#include <stdio.h>

/* Test creating and inserting attributes in different orders to ensure that
 * the list management code handles all cases. */
int checkAttrs(MPI_Comm comm, int n, int key[], int attrval[]);
int checkNoAttrs(MPI_Comm comm, int n, int key[]);


int copy_attr(MPI_Comm comm, int comm_keyval, void* extra_state, void* attribute_val_in, void* attribute_val_out, int* flag) {
    /* Call MPI_Comm_get_attr from inside the comm attribute copy callback.
     *
     * If YogiMPI has not wrapped the callback correctly, then the incoming `comm` will already
     * be a MPI_Comm instead of a YogiMPI_Comm, and when calling MPI_Comm_get_attr Yogi will try
     * to convert the comm and get a 'out_of_range' exception.
     *
     * If YogiMPI has correctly wrapped the callback, then the incoming `comm` will have been
     * converted back to a YogiMPI_Comm and the call to MPI_Comm_get_attr should succeed.
     *
     * This is a regression test. */
    MPI_Comm_get_attr(comm, comm_keyval, attribute_val_out, flag);

    /* Actually do the copy */
    *flag = 1;
    (*(void**)attribute_val_out) = attribute_val_in;
    return MPI_SUCCESS;
}


int delete_attr(MPI_Comm comm, int comm_keyval, void* attribute_val, void* extra_state) {
    /* Call MPI_Comm_get_attr from inside the comm attribute delete callback.
     *
     * If YogiMPI has not wrapped the callback correctly, then the incoming `comm` will already
     * be a MPI_Comm instead of a YogiMPI_Comm, and when calling MPI_Comm_get_attr Yogi will try
     * to convert the comm and get a 'out_of_range' exception.
     *
     * If YogiMPI has correctly wrapped the callback, then the incoming `comm` will have been
     * converted back to a YogiMPI_Comm and the call to MPI_Comm_get_attr should succeed.
     *
     * This is a regression test. */
    int flag;
    MPI_Comm_get_attr(comm, comm_keyval, attribute_val, &flag);

    return MPI_SUCCESS;
}


int main( int argc, char *argv[] )
{
    int errs = 0;
    int key[3], attrval[3], custom_key_copy, custom_key_del;
    int custom_val = 512;
    int i;
    MPI_Comm comm, comm_dup;

    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    /* Create key values */
    for (i=0; i<3; i++) {
        MPI_Comm_create_keyval(MPI_COMM_NULL_COPY_FN, MPI_COMM_NULL_DELETE_FN, &key[i], (void *)0 );
        attrval[i] = 1024 * i;
    }

    /* Insert attribute in several orders. Test after put with get, then delete, then confirm delete with get. */
    MPI_Comm_set_attr( comm, key[2], &attrval[2] );
    MPI_Comm_set_attr( comm, key[1], &attrval[1] );
    MPI_Comm_set_attr( comm, key[0], &attrval[0] );
    errs += checkAttrs( comm, 3, key, attrval );

    MPI_Comm_delete_attr( comm, key[0] );
    MPI_Comm_delete_attr( comm, key[1] );
    MPI_Comm_delete_attr( comm, key[2] );
    errs += checkNoAttrs( comm, 3, key );

    MPI_Comm_set_attr( comm, key[1], &attrval[1] );
    MPI_Comm_set_attr( comm, key[2], &attrval[2] );
    MPI_Comm_set_attr( comm, key[0], &attrval[0] );
    errs += checkAttrs( comm, 3, key, attrval );

    MPI_Comm_delete_attr( comm, key[2] );
    MPI_Comm_delete_attr( comm, key[1] );
    MPI_Comm_delete_attr( comm, key[0] );
    errs += checkNoAttrs( comm, 3, key );

    MPI_Comm_set_attr( comm, key[0], &attrval[0] );
    MPI_Comm_set_attr( comm, key[1], &attrval[1] );
    MPI_Comm_set_attr( comm, key[2], &attrval[2] );
    errs += checkAttrs( comm, 3, key, attrval );

    MPI_Comm_delete_attr( comm, key[1] );
    MPI_Comm_delete_attr( comm, key[2] );
    MPI_Comm_delete_attr( comm, key[0] );
    errs += checkNoAttrs( comm, 3, key );

    for (i=0; i<3; i++) {
        MPI_Comm_free_keyval( &key[i] );
    }

    /* Add a key with a custom copier, then see if horrible things happen when we copy the
     * communicator */
    MPI_Comm_create_keyval(&copy_attr, MPI_COMM_NULL_DELETE_FN, &custom_key_copy, (void *)0 );
    MPI_Comm_set_attr( comm, custom_key_copy, &custom_val );
    MPI_Comm_dup( comm, &comm_dup );

    /* Add a key with a custom deleter, then see if horrible things happen during finalize */
    MPI_Comm_create_keyval(MPI_COMM_NULL_COPY_FN, &delete_attr, &custom_key_del, (void *)0 );
    MPI_Comm_set_attr( comm, custom_key_del, &custom_val );

    MPI_Finalize();
    return errs;
}

int checkAttrs( MPI_Comm comm, int n, int key[], int attrval[] )
{
    int errs = 0;
    int i, flag, *val_p;
    for (i=0; i<n; i++) {
        MPI_Comm_get_attr( comm, key[i], &val_p, &flag );
        if (!flag) {
            errs++;
            fprintf( stderr, "Attribute for key %d not set\n", i );fflush(stderr);
        }
        else if (val_p != &attrval[i]) {
            errs++;
            fprintf( stderr, "Atribute value for key %d not correct\n", i );fflush(stderr);
        }
    }
    return errs;
}

int checkNoAttrs( MPI_Comm comm, int n, int key[] )
{
    int errs = 0;
    int i, flag, *val_p;
    for (i=0; i<n; i++) {
        MPI_Comm_get_attr( comm, key[i], &val_p, &flag );
        if (flag) {
            errs++;
            fprintf( stderr, "Attribute for key %d set but should be deleted\n", i );fflush(stderr);
        }
    }
    return errs;
}
