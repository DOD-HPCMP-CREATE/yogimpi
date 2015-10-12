/*
YogiMPI Library - MPI ABI Translator
Copyright (C) 2006, 2007 Toon Knapen Free Field Technologies S.A.
Additions made by Stephen Adamec, University of Alabama at Birmingham

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef YOGIMPI_H 
#define YOGIMPI_H
#ifdef __cplusplus
extern "C" {
#endif

#define YogiMPI_VERSION    2
#define YogiMPI_SUBVERSION 2

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
typedef int YogiMPI_Errhandler;

/* MPI constants for return codes (both C and Fortran) */
#define YogiMPI_SUCCESS 0 
#define YogiMPI_ERR_BUFFER 1 
#define YogiMPI_ERR_COUNT 2 
#define YogiMPI_ERR_TYPE 3 
#define YogiMPI_ERR_TAG 4 
#define YogiMPI_ERR_COMM 5 
#define YogiMPI_ERR_RANK 6 
#define YogiMPI_ERR_REQUEST 7 
#define YogiMPI_ERR_ROOT 8 
#define YogiMPI_ERR_GROUP 9 
#define YogiMPI_ERR_OP 10 
#define YogiMPI_ERR_TOPOLOGY 11 
#define YogiMPI_ERR_DIMS 12 
#define YogiMPI_ERR_ARG 13 
#define YogiMPI_ERR_UNKNOWN 14 
#define YogiMPI_ERR_TRUNCATE 15 
#define YogiMPI_ERR_OTHER 16 
#define YogiMPI_ERR_INTERN 17 
#define YogiMPI_ERR_PENDING 18 
#define YogiMPI_ERR_IN_STATUS 19 

/* Part 2 for MPI I/O error constants */

#define YogiMPI_ERR_FILE 20
#define YogiMPI_ERR_NOT_SAME 21
#define YogiMPI_ERR_AMODE 22
#define YogiMPI_ERR_UNSUPPORTED_DATAREP 23
#define YogiMPI_ERR_UNSUPPORTED_OPERATION 24
#define YogiMPI_ERR_NO_SUCH_FILE 25
#define YogiMPI_ERR_FILE_EXISTS 26
#define YogiMPI_ERR_BAD_FILE 27
#define YogiMPI_ERR_ACCESS 28
#define YogiMPI_ERR_NO_SPACE 29
#define YogiMPI_ERR_QUOTA 30
#define YogiMPI_ERR_READ_ONLY 31
#define YogiMPI_ERR_FILE_IN_USE 32
#define YogiMPI_ERR_DUP_DATAREP 33
#define YogiMPI_ERR_CONVERSION 34
#define YogiMPI_ERR_IO 35
#define YogiMPI_ERR_LASTCODE 36

/* Assorted constants (both C and Fortran) */

/* Recommended that MPI_BOTTOM be zero address */
static void * const  YogiMPI_BOTTOM = (void*)0;
#define YogiMPI_PROC_NULL -2
#define YogiMPI_ANY_SOURCE -3
/* ANY_TAG should not overlap with any valid tag */
#define YogiMPI_ANY_TAG -4
/* UNDEFINED should not overlap with any valid rank */
#define YogiMPI_UNDEFINED -5
#define YogiMPI_BSEND_OVERHEAD 512
#define YogiMPI_KEYVAL_INVALID -7

/* MPI asserts for one-sided communication.  Supports bitwise OR. */
#define YogiMPI_MODE_NOCHECK 1024
#define YogiMPI_MODE_NOSTORE 2048
#define YogiMPI_MODE_NOPUT 4096
#define YogiMPI_MODE_NOPRECEDE 8192
#define YogiMPI_MODE_NOSUCCEED 16384

/* Seeking constants. */
#define YogiMPI_SEEK_SET 50
#define YogiMPI_SEEK_CUR 55
#define YogiMPI_SEEK_END 60

/* MPI I/O permission modes.  Supports bitwise OR. 2*/
#define YogiMPI_MODE_RDONLY 2
#define YogiMPI_MODE_RDWR 8
#define YogiMPI_MODE_WRONLY 4
#define YogiMPI_MODE_CREATE 1
#define YogiMPI_MODE_EXCL 64
#define YogiMPI_MODE_DELETE_ON_CLOSE 16
#define YogiMPI_MODE_UNIQUE_OPEN 32
#define YogiMPI_MODE_SEQUENTIAL 256
#define YogiMPI_MODE_APPEND 128

/* MPI_Info have key and value length constants.  Pick conservative values
   that accommodate OpenMPI and MPICH derivatives. */
#define YogiMPI_MAX_INFO_KEY 32
#define YogiMPI_MAX_INFO_VAL 256

/* Subarray and darray constructor constants.  Mimics the MPICH header file */
#define YogiMPI_ORDER_C              56
#define YogiMPI_ORDER_FORTRAN        57
#define YogiMPI_DISTRIBUTE_BLOCK    121
#define YogiMPI_DISTRIBUTE_CYCLIC   122
#define YogiMPI_DISTRIBUTE_NONE     123
#define YogiMPI_DISTRIBUTE_DFLT_DARG -49767

/* The MPI-standard requires that the user provides an array of at least the
 * size equal to MAX_PROCESSOR_NAME.
 * However, YogiMPI will always return a null-terminated string. Therefore the
 * array has maximally MAX_PROCESSOR_NAME-1 useful characters. A 
 * null-terminator will thus be at the index MAX_PROCESSOR_NAME-1 or before.
 * If the underlying MPI implementation returns strings that are longer than
 * YogiMPI_MAX_PROCESSOR_NAME-1, the string will be truncated.
 */
#define YogiMPI_MAX_PROCESSOR_NAME 32
#define YogiMPI_MAX_ERROR_STRING 32 

/* Elementary datatypes (C) */ 
#define YogiMPI_CHAR 1
#define YogiMPI_SHORT 2
#define YogiMPI_INT 3
#define YogiMPI_LONG 4
#define YogiMPI_UNSIGNED_CHAR 5
#define YogiMPI_UNSIGNED_SHORT 6
#define YogiMPI_UNSIGNED 7
#define YogiMPI_UNSIGNED_LONG 8
#define YogiMPI_FLOAT 9
#define YogiMPI_DOUBLE 10
#define YogiMPI_LONG_DOUBLE 11
#define YogiMPI_BYTE 12
#define YogiMPI_PACKED 13

/* Datatypes for reduction functions (C) */

#define YogiMPI_FLOAT_INT 14
#define YogiMPI_DOUBLE_INT 15
#define YogiMPI_LONG_INT 16
#define YogiMPI_2INT 17
#define YogiMPI_SHORT_INT 18
#define YogiMPI_LONG_DOUBLE_INT 19

/* Optional datatypes (C) */ 
#define YogiMPI_LONG_LONG_INT 20
#define YogiMPI_INT32_T 21
#define YogiMPI_INT64_T 22

/* Make LONG_LONG a LONG_LONG_INT */
#define YogiMPI_LONG_LONG 20

/* Non-standard datatypes that are often in MPI implementations,
   so include them here.  Some of these are normally Fortran-only, but
   a few misbehaving (but required) C applications demand their presence. */
#define YogiMPI_COMPLEX 23
#define YogiMPI_DOUBLE_COMPLEX 24
#define YogiMPI_LOGICAL 25
#define YogiMPI_2REAL 26
#define YogiMPI_2DOUBLE_PRECISION 27
#define YogiMPI_2INTEGER 28
#define YogiMPI_INTEGER1 29
#define YogiMPI_INTEGER2 30
#define YogiMPI_INTEGER4 31
#define YogiMPI_INTEGER8 32
#define YogiMPI_REAL4 33
#define YogiMPI_REAL8 34
#define YogiMPI_UNSIGNED_LONG_LONG 35

/* Some Fortran-only definitions that end up matching byte-sizes with standard
   C types (at least on standard x86_64) */
#define YogiMPI_CHARACTER 1
#define YogiMPI_INTEGER 3
#define YogiMPI_REAL 33
#define YogiMPI_DOUBLE_PRECISION 10

/* Special datatypes for constructing derived datatypes */ 
#define YogiMPI_LB 36 
#define YogiMPI_UB 37

/* reserved communicators (C and Fortran) */
#define YogiMPI_COMM_WORLD 1
#define YogiMPI_COMM_SELF 2

/* results of communicator and group comparisons */
#define YogiMPI_IDENT 0
#define YogiMPI_CONGRUENT 1
#define YogiMPI_SIMILAR 2
#define YogiMPI_UNEQUAL 3

/* environmental inquiry keys (C and Fortran) */ 
#define YogiMPI_TAG_UB -8
#define YogiMPI_IO -9
#define YogiMPI_HOST -10
#define YogiMPI_WTIME_IS_GLOBAL -11

/* collective operations (C and Fortran) */ 
#define YogiMPI_MAX 1
#define YogiMPI_MIN 2
#define YogiMPI_SUM 3
#define YogiMPI_PROD 4
#define YogiMPI_MAXLOC 5
#define YogiMPI_MINLOC 6
#define YogiMPI_BAND 7
#define YogiMPI_BOR 8
#define YogiMPI_BXOR 9
#define YogiMPI_LAND 10
#define YogiMPI_LOR 11
#define YogiMPI_LXOR 12

/* Null handles */ 
#define YogiMPI_GROUP_NULL 0
#define YogiMPI_COMM_NULL 0
#define YogiMPI_DATATYPE_NULL 0
#define YogiMPI_REQUEST_NULL 0
#define YogiMPI_OP_NULL 0
#define YogiMPI_INFO_NULL 0
#define YogiMPI_FILE_NULL 0
#define YogiMPI_ERRHANDLER_NULL 0

/* Predefined error handlers */
#define YogiMPI_ERRORS_ARE_FATAL 1
#define YogiMPI_ERRORS_RETURN 2

/* Empty group */
#define YogiMPI_GROUP_EMPTY 1

/* MPI Combiner state constants */
#define YogiMPI_COMBINER_NAMED 1
#define YogiMPI_COMBINER_DUP 2
#define YogiMPI_COMBINER_CONTIGUOUS 3
#define YogiMPI_COMBINER_VECTOR 4
#define YogiMPI_COMBINER_HVECTOR_INTEGER 5
#define YogiMPI_COMBINER_HVECTOR 6
#define YogiMPI_COMBINER_INDEXED 7
#define YogiMPI_COMBINER_HINDEXED_INTEGER 8
#define YogiMPI_COMBINER_HINDEXED 9
#define YogiMPI_COMBINER_INDEXED_BLOCK 10
#define YogiMPI_COMBINER_STRUCT_INTEGER 12
#define YogiMPI_COMBINER_STRUCT 13
#define YogiMPI_COMBINER_SUBARRAY 14
#define YogiMPI_COMBINER_DARRAY 15
#define YogiMPI_COMBINER_F90_REAL 16
#define YogiMPI_COMBINER_F90_COMPLEX 17
#define YogiMPI_COMBINER_F90_INTEGER 18
#define YogiMPI_COMBINER_RESIZED 19

/* Define structure for MPI_Status - hide real object inside as int array */
struct YogiMPI_Status
{
  int MPI_SOURCE;
  int MPI_TAG;
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

/* Prototype function pointers for MPI. */
typedef void YogiMPI_User_function(void *invec, void *inoutvec,
                               int *len, YogiMPI_Datatype *datatype);

/* Note that we don't define extra stuff like other MPI implementations.
 * This gets converted internally to the implementation's varargs function
 * pointer version.
 */
typedef void YogiMPI_Comm_errhandler_function(YogiMPI_Comm *, int *, ...);

typedef int YogiMPI_Copy_function(YogiMPI_Comm oldcomm, int keyval,
                                  void *extra_state, void *attribute_val_in,
                                  void *attribute_val_out, int *flag);

typedef int YogiMPI_Delete_function(YogiMPI_Comm comm, int keyval,
                                    void *attribute_val, void *extra_state);

#define YogiMPI_NULL_COPY_FN   ((YogiMPI_Copy_function *)0)
#define YogiMPI_DUP_FN  ((YogiMPI_Copy_function *)-1)
#define YogiMPI_NULL_DELETE_FN ((YogiMPI_Delete_function *)0)

/* For MPI functions, occasionally they may need to resolve an MPI_Datatype
 * or MPI_Comm sent.  It's difficult to do the conversion internally, so 
 * users may have to modify source to make these calls in the function.  If the
 * the MPI_Datatype or MPI_Comm is never used, this isn't required.
 */
YogiMPI_Comm Yogi_ResolveComm(void *commObject);
YogiMPI_Datatype Yogi_ResolveDatatype(void *datatypeObject);
int Yogi_ResolveErrorcode(int errorcode);

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

YogiMPI_Datatype YogiMPI_Type_f2c(YogiMPI_Fint type);

int YogiMPI_File_read_all(YogiMPI_File mpi_fh, void* buf, int count, 
                          YogiMPI_Datatype datatype, YogiMPI_Status* status);

int YogiMPI_Type_get_extent(YogiMPI_Datatype datatype, YogiMPI_Aint* lb,
                            YogiMPI_Aint* extent);

int YogiMPI_Cancel(YogiMPI_Request* request);

int YogiMPI_Attr_put(YogiMPI_Comm comm, int keyval, void* attr_value);

int YogiMPI_Buffer_attach(void* buffer, int size);

int YogiMPI_Rsend(void* buf, int count, YogiMPI_Datatype datatype, int dest,
                  int tag, YogiMPI_Comm comm);

int YogiMPI_Unpack(void* inbuf, int insize, int* position, void* outbuf, 
                   int outcount, YogiMPI_Datatype datatype, YogiMPI_Comm comm);

int YogiMPI_Buffer_detach(void* buffer, int* size);

int YogiMPI_Bsend(void* buf, int count, YogiMPI_Datatype datatype, int dest,
                  int tag, YogiMPI_Comm comm);

int YogiMPI_Pack_size(int incount, YogiMPI_Datatype datatype, YogiMPI_Comm comm,
                      int* size);

int YogiMPI_Intercomm_create(YogiMPI_Comm local_comm, int local_leader,
                             YogiMPI_Comm peer_comm, int remote_leader,
                             int tag, YogiMPI_Comm* newintercomm);

int YogiMPI_Keyval_free(int* keyval);

int YogiMPI_Pack(void* inbuf, int incount, YogiMPI_Datatype datatype,
                 void* outbuf, int outcount, int* position, YogiMPI_Comm comm);

int YogiMPI_Group_range_excl(YogiMPI_Group group, int n, int ranges[][3],
                             YogiMPI_Group* newgroup);

int YogiMPI_Type_create_hindexed(int count, int blocklengths[],
                                 YogiMPI_Aint displacements[],
                                 YogiMPI_Datatype oldtype,
                                 YogiMPI_Datatype* newtype);

int YogiMPI_Pcontrol(int level);

int YogiMPI_Error_class(int errorcode, int* errorclass);

int YogiMPI_Irsend(void* buf, int count, YogiMPI_Datatype datatype, int dest,
                   int tag, YogiMPI_Comm comm, YogiMPI_Request* request);

double YogiMPI_Wtick();

int YogiMPI_Type_create_struct(int count, int array_of_blocklengths[],
                               YogiMPI_Aint array_of_displacements[],
                               YogiMPI_Datatype array_of_types[],
                               YogiMPI_Datatype* newtype);

int YogiMPI_Reduce_scatter(void* sendbuf, void* recvbuf, int* recvcnts, 
                           YogiMPI_Datatype datatype, YogiMPI_Op op,
                           YogiMPI_Comm comm);

int YogiMPI_Comm_compare(YogiMPI_Comm comm1, YogiMPI_Comm comm2, int* result);

int YogiMPI_Sendrecv(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype,
                     int dest, int sendtag, void* recvbuf, int recvcount,
                     YogiMPI_Datatype recvtype, int source, int recvtag, 
                     YogiMPI_Comm comm, YogiMPI_Status* status);

int YogiMPI_Group_size(YogiMPI_Group group, int* size);

int YogiMPI_Type_create_subarray(int ndims, int array_of_sizes[],
                                 int array_of_subsizes[], int array_of_starts[],
                                 int order, YogiMPI_Datatype oldtype,
                                 YogiMPI_Datatype* newtype);

int YogiMPI_Attr_delete(YogiMPI_Comm comm, int keyval);

int YogiMPI_Comm_remote_size(YogiMPI_Comm comm, int* size);

int YogiMPI_Get_address(void* location, YogiMPI_Aint* address);

int YogiMPI_Type_create_hvector(int count, int blocklength, YogiMPI_Aint stride,
		                        YogiMPI_Datatype oldtype,
                                YogiMPI_Datatype* newtype);

int YogiMPI_Errhandler_free(YogiMPI_Errhandler* errhandler);

int YogiMPI_Comm_get_errhandler(YogiMPI_Comm comm, 
		                        YogiMPI_Errhandler* errhandler);

int YogiMPI_Comm_set_errhandler(YogiMPI_Comm comm, 
		                        YogiMPI_Errhandler errhandler);

int YogiMPI_Errhandler_set(YogiMPI_Comm comm, YogiMPI_Errhandler errhandler);


int YogiMPI_Type_get_envelope(YogiMPI_Datatype datatype, int* num_integers, 
		                      int* num_addresses, int* num_datatypes, 
							  int* combiner);

int YogiMPI_Type_get_contents(YogiMPI_Datatype datatype, int max_integers,
		                      int max_addresses, int max_datatypes, 
							  int array_of_integers[], 
							  YogiMPI_Aint array_of_addresses[], 
							  YogiMPI_Datatype array_of_datatypes[]);

int YogiMPI_Op_free(YogiMPI_Op* op);

int YogiMPI_Cart_coords(YogiMPI_Comm comm, int rank, int maxdims, int coords[]);

int YogiMPI_Cart_create(YogiMPI_Comm comm_old, int ndims, int dims[], 
		                int periods[], int reorder, YogiMPI_Comm* comm_cart);

int YogiMPI_Dims_create(int nnodes, int ndims, int dims[]);

int YogiMPI_Info_get_valuelen(YogiMPI_Info info, char* key, int* valuelen,
		                      int* flag);

int YogiMPI_Type_create_darray(int size, int rank, int ndims,
		                       int array_of_gsizes[], int array_of_distribs[],
							   int array_of_dargs[], int array_of_psizes[],
							   int order, YogiMPI_Datatype oldtype,
							   YogiMPI_Datatype* newtype);

int YogiMPI_Type_create_resized(YogiMPI_Datatype oldtype, YogiMPI_Aint lb,
		                        YogiMPI_Aint extent, YogiMPI_Datatype* newtype);

int YogiMPI_Keyval_create(YogiMPI_Copy_function* copy_fn, 
		                  YogiMPI_Delete_function* delete_fn, int* keyval,
						  void* extra_state);

int YogiMPI_Comm_create_errhandler(YogiMPI_Comm_errhandler_function* function,
		                           YogiMPI_Errhandler* errhandler);

int YogiMPI_Op_create(YogiMPI_User_function* function, int commute, 
		              YogiMPI_Op* op);

int YogiMPI_Comm_call_errhandler(YogiMPI_Comm comm, int errorcode);

int YogiMPI_Testall(int count, YogiMPI_Request array_of_requests[],
                    int *flag, YogiMPI_Status array_of_statuses[]);

int YogiMPI_Test_cancelled(YogiMPI_Status *status, int *flag);

int YogiMPI_Group_range_incl(YogiMPI_Group group, int n, int ranges[][3],
                             YogiMPI_Group *newgroup);

int YogiMPI_Testany(int count, YogiMPI_Request *array_of_requests,
                    int *index, int *flag, YogiMPI_Status *status);

int YogiMPI_Testsome(int incount, YogiMPI_Request *array_of_requests,
                     int *outcount, int *array_of_indices, 
                     YogiMPI_Status *array_of_statuses);


#ifdef __cplusplus
}
#endif
#endif /* YOGIMPI_H */
