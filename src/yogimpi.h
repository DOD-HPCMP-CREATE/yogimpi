/*
 *  * Copyright Toon Knapen 2006, 2007
 *   * Copyright Free Field Technologies S.A. 2006, 2007
 *    */

/* Additions for MPI-2 by Stephen Adamec, CREATE Air Vehicles 2014 */

#ifndef YOGIMPI_H 
#define YOGIMPI_H 
#ifdef __cplusplus
extern "C" {
#endif

/*
 * MPI_Status can be treated as a series of integers (as in Fortran).
 * YogiMPI stores an integer array larger than any known MPI distribution's
 * MPI_Status byte size.
 */

#define MAX_STATUS_SIZE 12

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

/* MPI asserts for one-sided communication.  Supports bitwise OR. */
static const int YogiMPI_MODE_NOCHECK = 1024;
static const int YogiMPI_MODE_NOSTORE = 2048;
static const int YogiMPI_MODE_NOPUT = 4096;
static const int YogiMPI_MODE_NOPRECEDE = 8192;
static const int YogiMPI_MODE_NOSUCCEED = 16384;

/* Seeking constants. */
static const int YogiMPI_SEEK_SET = 50;
static const int YogiMPI_SEEK_CUR = 55;
static const int YogiMPI_SEEK_END = 60;

/* MPI I/O permission modes.  Supports bitwise OR. 2*/

static const int YogiMPI_MODE_RDONLY = 2;
static const int YogiMPI_MODE_RDWR = 8;
static const int YogiMPI_MODE_WRONLY = 4;
static const int YogiMPI_MODE_CREATE = 1;
static const int YogiMPI_MODE_EXCL = 64;
static const int YogiMPI_MODE_DELETE_ON_CLOSE = 16;
static const int YogiMPI_MODE_UNIQUE_OPEN = 32;
static const int YogiMPI_MODE_SEQUENTIAL = 256;
static const int YogiMPI_MODE_APPEND = 128;

/* MPI_Info have key and value length constants.  Pick conservative values
   that accommodate OpenMPI and MPICH derivatives. */

#define YogiMPI_MAX_INFO_KEY 32
#define YogiMPI_MAX_INFO_VAL 256

/* The MPI-standard requires that the user provides an array of at least the
 * size equal to MAX_PROCESSOR_NAME.
 * However, YogiMPI will always return a null-terminated string. Therefore the
 * array has maximally MAX_PROCESSOR_NAME-1 useful characters. A 
 * null-terminator will thus be at the index MAX_PROCESSOR_NAME-1 or before.
 * If the underlying MPI implementation returns strings that are longer than
 * YogiMPI_MAX_PROCESSOR_NAME-1, the string will be truncated.
 */
static const int YogiMPI_MAX_PROCESSOR_NAME = 32;
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

/* Special datatypes for constructing derived datatypes */ 
static const YogiMPI_Datatype YogiMPI_LB = 35; 
static const YogiMPI_Datatype YogiMPI_UB = 36;

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
static const YogiMPI_File YogiMPI_FILE_NULL = 0;

/* Empty group */
static const YogiMPI_Group YogiMPI_GROUP_EMPTY = 1;

/* Define structure for MPI_Status - hide real object inside as int array */
struct YogiMPI_Status
{
  int MPI_TAG;
  int MPI_SOURCE;
  int MPI_ERROR;
  int realStatus[MAX_STATUS_SIZE];
};

typedef struct YogiMPI_Status YogiMPI_Status;
static YogiMPI_Status * const YogiMPI_STATUS_IGNORE = 0;
static YogiMPI_Status * const YogiMPI_STATUSES_IGNORE = 0;

/* 
 * MPI_Aint is implementation dependent.  Pick a long long for it locally and
 * then cast to the actual type within the function.  In some instances
 * casting may not be necessary, which can be determined at runtime.
 */
typedef long long YogiMPI_Aint;

/* MPI_Offset is implementation dependent.  Pick a long long for it locally and
 * then cast to the actual type within the function.  In some instances 
 * casting may not be necessary, which can be determined at runtime.
 */
typedef long long YogiMPI_Offset;

/* MPI_Fint is implementation dependent.  Pick an int for it locally and
 * then cast to the actual type within the function.  In some instances 
 * casting may not be necessary, which can be determined at runtime.
 */
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
                  int tag, YogiMPI_Comm comm);

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

int YogiMPI_Waitsome(int incount, YogiMPI_Request *array_of_requests, 
		             int *outcount, int *array_of_indices, 
					 YogiMPI_Status *array_of_statuses);

int YogiMPI_Waitany(int count, YogiMPI_Request *array_of_requests, int *index, 
		            YogiMPI_Status *status);

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

int YogiMPI_Scatter(void *sendbuf, int sendcount, YogiMPI_Datatype sendtype,
                    void *recvbuf, int recvcount, YogiMPI_Datatype recvtype, 
					int root, YogiMPI_Comm comm);

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

int YogiMPI_Comm_create(YogiMPI_Comm comm, YogiMPI_Group group,
                        YogiMPI_Comm *newcomm);

int YogiMPI_Comm_group(YogiMPI_Comm comm, YogiMPI_Group* group);

int YogiMPI_Comm_size(YogiMPI_Comm comm, int* size);

int YogiMPI_Comm_rank(YogiMPI_Comm comm, int* rank);

int YogiMPI_Comm_dup(YogiMPI_Comm comm, YogiMPI_Comm* out);

int YogiMPI_Comm_split(YogiMPI_Comm comm, int color, int key,
                       YogiMPI_Comm* new_comm);

int YogiMPI_Comm_free(YogiMPI_Comm *comm);

int YogiMPI_Group_free(YogiMPI_Group *group);

int YogiMPI_Group_incl(YogiMPI_Group group, int n, int *ranks, 
		               YogiMPI_Group* group_out);

int YogiMPI_Group_rank(YogiMPI_Group group, int *rank);

int YogiMPI_Group_translate_ranks(YogiMPI_Group group1, int n, int *ranks1, 
		                          YogiMPI_Group group2, int *ranks2);

int YogiMPI_Get_processor_name(char *name, int *resultlen);

double YogiMPI_Wtime(void);

int YogiMPI_Init(int* argc, char ***argv);

int YogiMPI_Finalize(void);

int YogiMPI_Type_create_indexed_block(int count, int blocklength, 
                                      int array_of_displacements[],
                                      YogiMPI_Datatype oldtype,
                                      YogiMPI_Datatype *newtype);

YogiMPI_Comm YogiMPI_Comm_f2c(YogiMPI_Fint comm);

int YogiMPI_Recv_init(void *buf, int count, YogiMPI_Datatype datatype, 
		              int source, int tag, YogiMPI_Comm comm, 
					  YogiMPI_Request *request);

int YogiMPI_Scan(const void *sendbuf, void *recvbuf, int count, 
		         YogiMPI_Datatype datatype, YogiMPI_Op op, YogiMPI_Comm comm);

int YogiMPI_Startall(int count, YogiMPI_Request *array_of_requests);

int YogiMPI_Alltoall(const void *sendbuf, int sendcount, 
		             YogiMPI_Datatype sendtype, void *recvbuf, int recvcount,
					 YogiMPI_Datatype recvtype, YogiMPI_Comm comm);

int YogiMPI_Alltoallv(const void *sendbuf, const int *sendcounts,
                      const int *sdispls, YogiMPI_Datatype sendtype, 
					  void *recvbuf, const int *recvcounts, const int *rdispls,
					  YogiMPI_Datatype recvtype, YogiMPI_Comm comm);

int YogiMPI_File_close(YogiMPI_File *fh);

int YogiMPI_File_get_info(YogiMPI_File fh, YogiMPI_Info *info_used);

int YogiMPI_File_open(YogiMPI_Comm comm, char *filename, int amode, 
		              YogiMPI_Info info, YogiMPI_File *fh);

int YogiMPI_File_set_view(YogiMPI_File fh, YogiMPI_Offset disp, 
		                  YogiMPI_Datatype etype, YogiMPI_Datatype filetype,
                          const char *datarep, YogiMPI_Info info);

int YogiMPI_File_write_all(YogiMPI_File fh, const void *buf, int count, 
		                   YogiMPI_Datatype datatype, YogiMPI_Status *status);

int YogiMPI_File_write_at(YogiMPI_File fh, YogiMPI_Offset offset, 
		                  const void *buf, int count, YogiMPI_Datatype datatype, 
					      YogiMPI_Status *status);

int YogiMPI_Info_create(YogiMPI_Info *info);

int YogiMPI_Info_set(YogiMPI_Info info, char *key, char *value);

int YogiMPI_Test(YogiMPI_Request *request, int *flag, YogiMPI_Status *status);

int YogiMPI_Probe(int source, int tag, YogiMPI_Comm comm, 
                  YogiMPI_Status* status);

int YogiMPI_Iprobe(int source, int tag, YogiMPI_Comm comm, int *flag,
                   YogiMPI_Status *status);

int YogiMPI_Abort(YogiMPI_Comm comm, int errorcode);

int YogiMPI_Attr_get(YogiMPI_Comm comm, int keyval, void *attribute_val,
                     int *flag);

int YogiMPI_Initialized(int *flag);

int YogiMPI_Finalized(int *flag);

int YogiMPI_Info_get_nthkey(YogiMPI_Info info, int n, char* key);

int YogiMPI_File_write(YogiMPI_File fh, void* buf, int count, 
		               YogiMPI_Datatype datatype, YogiMPI_Status* status);

int YogiMPI_Info_dup(YogiMPI_Info info, YogiMPI_Info* newinfo);

int YogiMPI_File_read_at_all(YogiMPI_File mpi_fh, YogiMPI_Offset offset, 
		                     void* buf, int count, YogiMPI_Datatype datatype, 
							 YogiMPI_Status* status);

int YogiMPI_File_seek(YogiMPI_File mpi_fh, YogiMPI_Offset offset, int whence);

int YogiMPI_Type_struct(int count, int blocklens[], YogiMPI_Aint indices[], 
		                YogiMPI_Datatype old_types[], 
						YogiMPI_Datatype* newtype);

int YogiMPI_File_read_at(YogiMPI_File mpi_fh, YogiMPI_Offset offset, void* buf,
		                 int count, YogiMPI_Datatype datatype, 
						 YogiMPI_Status* status);

int YogiMPI_File_write_at_all(YogiMPI_File mpi_fh, YogiMPI_Offset offset, 
		                      void* buf, int count, YogiMPI_Datatype datatype, 
							  YogiMPI_Status* status);

int YogiMPI_Type_extent(YogiMPI_Datatype datatype, YogiMPI_Aint* extent);

int YogiMPI_Get_elements(YogiMPI_Status* status, YogiMPI_Datatype datatype, 
		                 int* elements);

int YogiMPI_Type_hindexed(int count, int blocklens[], YogiMPI_Aint indices[],
		                  YogiMPI_Datatype old_type,
						  YogiMPI_Datatype* newtype);

int YogiMPI_File_set_atomicity(YogiMPI_File mpi_fh, int flag);

int YogiMPI_File_sync(YogiMPI_File mpi_fh);

int YogiMPI_Type_hvector(int count, int blocklen, YogiMPI_Aint stride,
		                 YogiMPI_Datatype old_type, YogiMPI_Datatype* newtype);

int YogiMPI_File_delete(char* filename, YogiMPI_Info info);

int YogiMPI_Address(void* location, YogiMPI_Aint* address);

int YogiMPI_Info_free(YogiMPI_Info* info);

int YogiMPI_Info_get(YogiMPI_Info info, char* key, int valuelen, char* value,
		             int* flag);

int YogiMPI_Group_excl(YogiMPI_Group group, int n, int* ranks,
		               YogiMPI_Group* newgroup);

int YogiMPI_File_get_atomicity(YogiMPI_File mpi_fh, int* flag);

int YogiMPI_File_read(YogiMPI_File mpi_fh, void* buf, int count,
		              YogiMPI_Datatype datatype, YogiMPI_Status* status);

int YogiMPI_Info_get_nkeys(YogiMPI_Info info, int* nkeys);

int YogiMPI_File_get_size(YogiMPI_File fh, YogiMPI_Offset* size);

int YogiMPI_File_set_size(YogiMPI_File fh, YogiMPI_Offset size);

int YogiMPI_Error_string(int errorcode, char* string, int* resultlen);

YogiMPI_Comm YogiMPI_Comm_f2c(YogiMPI_Fint comm);

YogiMPI_Fint YogiMPI_Comm_c2f(YogiMPI_Comm comm);

YogiMPI_Info YogiMPI_Info_f2c(YogiMPI_Fint info);

YogiMPI_Fint YogiMPI_Info_c2f(YogiMPI_Info info);

#ifdef __cplusplus
}
#endif
#endif /* YOGIMPI_H */