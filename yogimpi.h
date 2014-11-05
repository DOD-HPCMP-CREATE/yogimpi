/*
 *  * Copyright Toon Knapen 2006, 2007
 *   * Copyright Free Field Technologies S.A. 2006, 2007
 *    */

/* Additions for MPI-2 by Stephen Adamec, CREATE Air Vehicles 2014 */

#ifndef YOGIMPI_H 
#define YOGIMPI_H 

/*
 * Typedefs for opaque MPI data structures.
 */
typedef int YogiMPI_Group;
typedef int YogiMPI_Comm;
typedef int YogiMPI_Datatype;
typedef int YogiMPI_Request;
typedef int YogiMPI_Op;
typedef int YogiMPI_Info;
typedef int YogiMPI_File;

/* MPI constants for return codes (both C and Fortran) */
static const int YogiMPI_SUCCESS = 0; 
static const int YogiMPI_ERR_BUFFER = 1; 
static const int YogiMPI_ERR_COUNT = 2; 
static const int YogiMPI_ERR_TYPE = 3; 
static const int YogiMPI_ERR_TAG = 4; 
static const int YogiMPI_ERR_COMM = 5; 
static const int YogiMPI_ERR_RANK = 6; 
static const int YogiMPI_ERR_REQUEST = 7; 
static const int YogiMPI_ERR_ROOT = 8; 
static const int YogiMPI_ERR_GROUP = 9; 
static const int YogiMPI_ERR_OP = 10; 
static const int YogiMPI_ERR_TOPOLOGY = 11; 
static const int YogiMPI_ERR_DIMS = 12; 
static const int YogiMPI_ERR_ARG = 13; 
static const int YogiMPI_ERR_UNKNOWN = 14; 
static const int YogiMPI_ERR_TRUNCATE = 15; 
static const int YogiMPI_ERR_OTHER = 16; 
static const int YogiMPI_ERR_INTERN = 17; 
static const int YogiMPI_ERR_PENDING = 18; 
static const int YogiMPI_ERR_IN_STATUS = 19; 
static const int YogiMPI_ERR_LASTCODE = 20; 

/* Assorted constants (both C and Fortran) */

/* Recommended that MPI_BOTTOM be zero address */
static void * const  YogiMPI_BOTTOM = (void*)0 ;
static const int YogiMPI_PROC_NULL = -2;
static const int YogiMPI_ANY_SOURCE = -3;
/* ANY_TAG should not overlap with any valid tag */
static const int YogiMPI_ANY_TAG = -4;
/* UNDEFINED should not overlap with any valid rank */
static const int YogiMPI_UNDEFINED = -5;
static const int YogiMPI_BSEND_OVERHEAD = 512;
static const int YogiMPI_KEYVAL_INVALID = -7;

/* MPI I/O permission modes, powers of 2*/

static const int YogiMPI_MODE_RDONLY = 2;
static const int YogiMPI_MODE_RDWR = 8;
static const int YogiMPI_MODE_WRONLY = 4;
static const int YogiMPI_MODE_CREATE = 1;
static const int YogiMPI_MODE_EXCL = 64;
static const int YogiMPI_MODE_DELETE_ON_CLOSE = 16;
static const int YogiMPI_MODE_UNIQUE_OPEN = 32;
static const int YogiMPI_MODE_SEQUENTIAL = 256;
static const int YogiMPI_MODE_APPEND = 128;

/* The MPI-standard requires that the user provides an array of at least the
 * size equal to MAX_PROCESSOR_NAME.
 * However, YogiMPI will always return a null-terminated string. Therefore the
 * array has maximally MAX_PROCESSOR_NAME-1 useful characters. A 
 * null-terminator will thus be at the index MAX_PROCESSOR_NAME-1 or before.
 * If the underlying MPI implementation returns strings that are longer than
 * YogiMPI_MAX_PROCESSOR_NAME-1, the string will be truncated.
 */
static const int YogiMPI_MAX_PROCESSOR_NAME = 32 ;
#define YogiMPI_MAX_ERROR_STRING 32 

/* Elementary datatypes (C) */ 
static const YogiMPI_Datatype YogiMPI_CHAR = 1;
static const YogiMPI_Datatype YogiMPI_SHORT = 2;
static const YogiMPI_Datatype YogiMPI_INT = 3;
static const YogiMPI_Datatype YogiMPI_LONG = 4;
static const YogiMPI_Datatype YogiMPI_UNSIGNED_CHAR = 5;
static const YogiMPI_Datatype YogiMPI_UNSIGNED_SHORT = 6;
static const YogiMPI_Datatype YogiMPI_UNSIGNED = 7;
static const YogiMPI_Datatype YogiMPI_UNSIGNED_LONG = 8;
static const YogiMPI_Datatype YogiMPI_FLOAT = 9;
static const YogiMPI_Datatype YogiMPI_DOUBLE = 10;
static const YogiMPI_Datatype YogiMPI_LONG_DOUBLE = 11;
static const YogiMPI_Datatype YogiMPI_BYTE = 12;
static const YogiMPI_Datatype YogiMPI_PACKED = 13;

/* Datatypes for reduction functions (C) */

static const YogiMPI_Datatype YogiMPI_FLOAT_INT = 14;
static const YogiMPI_Datatype YogiMPI_DOUBLE_INT = 15;
static const YogiMPI_Datatype YogiMPI_LONG_INT = 16;
static const YogiMPI_Datatype YogiMPI_2INT = 17;
static const YogiMPI_Datatype YogiMPI_SHORT_INT = 18;
static const YogiMPI_Datatype YogiMPI_LONG_DOUBLE_INT = 19;

/* Optional datatypes (C) */ 
static const YogiMPI_Datatype YogiMPI_LONG_LONG_INT = 20;
static const YogiMPI_Datatype YogiMPI_INT32_T = 21;
static const YogiMPI_Datatype YogiMPI_INT64_T = 22;

/* reserved communicators (C and Fortran) */
static const YogiMPI_Comm YogiMPI_COMM_WORLD = 1;
static const YogiMPI_Comm YogiMPI_COMM_SELF = 2;

/* results of communicator and group comparisons */
static const int YogiMPI_IDENT = 0;
static const int YogiMPI_CONGRUENT = 1;
static const int YogiMPI_SIMILAR = 2;
static const int YogiMPI_UNEQUAL = 3;

/* environmental inquiry keys (C and Fortran) */ 
static const int YogiMPI_TAG_UB = -8;
static const int YogiMPI_IO = -9;
static const int YogiMPI_HOST = -10;
static const int YogiMPI_WTIME_IS_GLOBAL = -11;

/* collective operations (C and Fortran) */ 
static const YogiMPI_Op YogiMPI_MAX = 1;
static const YogiMPI_Op YogiMPI_MIN = 2;
static const YogiMPI_Op YogiMPI_SUM = 3;
static const YogiMPI_Op YogiMPI_PROD = 4;
static const YogiMPI_Op YogiMPI_MAXLOC = 5;
static const YogiMPI_Op YogiMPI_MINLOC = 6;
static const YogiMPI_Op YogiMPI_BAND = 7;
static const YogiMPI_Op YogiMPI_BOR = 8;
static const YogiMPI_Op YogiMPI_BXOR = 9;
static const YogiMPI_Op YogiMPI_LAND = 10;
static const YogiMPI_Op YogiMPI_LOR = 11;
static const YogiMPI_Op YogiMPI_LXOR = 12;

/* Null handles */ 
static const YogiMPI_Group YogiMPI_GROUP_NULL = 0; 
static const YogiMPI_Comm YogiMPI_COMM_NULL = 0;
static const YogiMPI_Datatype YogiMPI_DATATYPE_NULL = 0;
static const YogiMPI_Request YogiMPI_REQUEST_NULL = 0;
static const YogiMPI_Op YogiMPI_OP_NULL = 0;
static const YogiMPI_Info YogiMPI_INFO_NULL = 0;

/* Empty group */
static const YogiMPI_Group YogiMPI_GROUP_EMPTY = 1;

/* Define structure for MPI_Status - hide real object inside as void pointer */
struct YogiMPI_Status
{
  int YogiMPI_SOURCE;
  int YogiMPI_TAG;
  int YogiMPI_ERROR;
  void * mpi_status;
};

typedef struct YogiMPI_Status YogiMPI_Status;
static YogiMPI_Status * const YogiMPI_STATUS_IGNORE = 0;
static YogiMPI_Status * const YogiMPI_STATUSES_IGNORE = 0;

/* 
 * Currently aligned with MPICH2 and OpenMPI - TODO ./configure to check 
 */
typedef long YogiMPI_Aint;

/* MPI 2 definition * - TODO ./configure to check */
typedef int YogiMPI_Fint;

static void * const YogiMPI_IN_PLACE = (void *)-1;

/* Now prototype functions. */

int YogiMPI_Send(void* buf, int count, YogiMPI_Datatype datatype, int dest,
                 int tag, YogiMPI_Comm comm);

int YogiMPI_Recv(void* buf, int count, YogiMPI_Datatype datatype, int source,
                 int tag, YogiMPI_Comm comm, YogiMPI_Status *status);

int YogiMPI_Get_count(YogiMPI_Status *status, YogiMPI_Datatype datatype, 
                      int *count);

int YogiMPI_Ssend(void* buf, int count, YogiMPI_Datatype datatype, int dest,
                  int tag, YogiMPI_Comm comm) ;

int YogiMPI_Isend(void *buf, int count, YogiMPI_Datatype datatype, int dest,
                  int tag, YogiMPI_Comm comm, YogiMPI_Request* request);

int YogiMPI_Issend(void *buf, int count, YogiMPI_Datatype datatype, int dest,
                   int tag, YogiMPI_Comm comm, YogiMPI_Request* request);

int YogiMPI_Irecv(void *buf, int count, YogiMPI_Datatype datatype, int source,
                  int tag, YogiMPI_Comm comm, YogiMPI_Request *request);

int YogiMPI_Wait(YogiMPI_Request* request, YogiMPI_Status* status);

int YogiMPI_Request_free(YogiMPI_Request *request);

int YogiMPI_Waitall(int count, YogiMPI_Request *array_of_requests,
                    YogiMPI_Status *array_of_statuses);

int YogiMPI_Send_init(void *buf, int count, YogiMPI_Datatype datatype, int dest,
                      int tag, YogiMPI_Comm comm, YogiMPI_Request* request);

int YogiMPI_Type_size(const YogiMPI_Datatype datatype, int * size);

int YogiMPI_Type_contiguous(int count, YogiMPI_Datatype oldtype, 
                            YogiMPI_Datatype *newtype);

int YogiMPI_Type_vector(int count, int blocklength, int stride, 
                        YogiMPI_Datatype oldtype, YogiMPI_Datatype *newtype);

int YogiMPI_Type_indexed(int count, int *array_of_blocklengths, 
                         int *array_of_displacements, YogiMPI_Datatype oldtype,
                         YogiMPI_Datatype *newtype);

int YogiMPI_Type_commit(YogiMPI_Datatype *datatype);

int YogiMPI_Type_free(YogiMPI_Datatype *datatype);

int YogiMPI_Barrier(YogiMPI_Comm comm);

int YogiMPI_Bcast(void* buffer, int count, YogiMPI_Datatype datatype, int root,
                  YogiMPI_Comm comm);

int YogiMPI_Gather(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype,
                   void* recvbuf, int recvcount, YogiMPI_Datatype recvtype,
                   int root, YogiMPI_Comm comm);

int YogiMPI_Gatherv(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype, 
                    void* recvbuf, int *recvcounts, int *displs, 
                    YogiMPI_Datatype recvtype, int root, YogiMPI_Comm comm);

int YogiMPI_Scatterv(void* sendbuf, int *sendcounts, int *displs, 
                     YogiMPI_Datatype sendtype, void* recvbuf, int recvcount,
                     YogiMPI_Datatype recvtype, int root, YogiMPI_Comm comm);

int YogiMPI_Allgather(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype,
                      void* recvbuf, int recvcount, YogiMPI_Datatype recvtype,
                      YogiMPI_Comm comm);

int YogiMPI_Allgatherv(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype,
                       void* recvbuf, int* recvcounts, int* displs, 
                       YogiMPI_Datatype recvtype, YogiMPI_Comm comm);

int YogiMPI_Reduce(void* sendbuf, void* recvbuf, int count, 
                   YogiMPI_Datatype datatype, YogiMPI_Op op, int root,
                   YogiMPI_Comm comm);

int YogiMPI_Allreduce(void* sendbuf, void* recvbuf, int count,
                      YogiMPI_Datatype datatype, YogiMPI_Op op,
                      YogiMPI_Comm comm);

int YogiMPI_Comm_size(YogiMPI_Comm comm, int* size);

int YogiMPI_Comm_rank(YogiMPI_Comm comm, int* rank);

int YogiMPI_Comm_dup(YogiMPI_Comm comm, YogiMPI_Comm* out);

int YogiMPI_Comm_split(YogiMPI_Comm comm, int color, int key,
                       YogiMPI_Comm* new_comm);

int YogiMPI_Comm_free(YogiMPI_Comm *comm);

int YogiMPI_Get_processor_name(char *name, int *resultlen);

double YogiMPI_Wtime();

int YogiMPI_Init(int* argc, char ***argv);

int YogiMPI_Finalize();

int YogiMPI_Type_create_indexed_block(int count, int blocklength, 
                                      int array_of_displacements[],
                                      YogiMPI_Datatype oldtype,
                                      YogiMPI_Datatype *newtype);

YogiMPI_Comm YogiMPI_Comm_f2c(YogiMPI_Fint comm);

#endif /* YOGIMPI_H */
