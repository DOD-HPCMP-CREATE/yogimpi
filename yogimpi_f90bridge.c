/* File to bind YogiMPI C functions to Fortran. */

#include "yogimpi.h"
#include <stdlib.h>

/* Define how the C functions will appear to Fortran.  Typically this is all
 * lowercase, with an additional underscore at the end.
 */
#define YOGIMPI_SEND yogimpi_send_
#define YOGIMPI_RECV yogimpi_recv_
#define YOGIMPI_GET_COUNT yogimpi_get_count_
#define YOGIMPI_SSEND yogimpi_ssend_
#define YOGIMPI_ISEND yogimpi_isend_
#define YOGIMPI_ISSEND yogimpi_issend_
#define YOGIMPI_IRECV yogimpi_irecv_
#define YOGIMPI_TYPE_VECTOR yogimpi_type_vector_
#define YOGIMPI_TYPE_CONTIGUOUS yogimpi_type_contiguous_
#define YOGIMPI_TYPE_INDEXED yogimpi_type_indexed_
#define YOGIMPI_TYPE_SIZE yogimpi_type_size_
#define YOGIMPI_TYPE_COMMIT yogimpi_type_commit_
#define YOGIMPI_TYPE_FREE yogimpi_type_free_
#define YOGIMPI_WAIT yogimpi_wait_
#define YOGIMPI_REQUEST_FREE yogimpi_request_free_
#define YOGIMPI_BARRIER yogimpi_barrier_
#define YOGIMPI_BCAST yogimpi_bcast_
#define YOGIMPI_GATHER yogimpi_gather_
#define YOGIMPI_REDUCE yogimpi_reduce_
#define YOGIMPI_ALLREDUCE yogimpi_allreduce_
#define YOGIMPI_COMM_SIZE yogimpi_comm_size_
#define YOGIMPI_COMM_RANK yogimpi_comm_rank_
#define YOGIMPI_COMM_DUP yogimpi_comm_dup_
#define YOGIMPI_COMM_SPLIT yogimpi_comm_split_
#define YOGIMPI_COMM_FREE yogimpi_comm_free_
#define YOGIMPI_GET_PROCESSOR_NAME yogimpi_get_processor_name_
#define YOGIMPI_WTIME yogimpi_wtime_
#define YOGIMPI_INIT yogimpi_init_
#define YOGIMPI_FINALIZE yogimpi_finalize_
#define YOGIMPI_TYPE_CREATE_INDEXED_BLOCK yogimpi_type_create_indexed_block_
#define YOGIMPI_ALLTOALLV yogimpi_alltoallv_
#define YOGIMPI_ALLTOALL yogimpi_alltoall_
#define YOGIMPI_STARTALL yogimpi_startall_
#define YOGIMPI_SCAN yogimpi_scan_
#define YOGIMPI_FILE_CLOSE yogimpi_file_close_
#define YOGIMPI_FILE_GET_INFO yogimpi_file_get_info_
#define YOGIMPI_FILE_OPEN yogimpi_file_open_
#define YOGIMPI_FILE_SET_VIEW yogimpi_file_set_view_
#define YOGIMPI_FILE_WRITE_ALL yogimpi_file_write_all_
#define YOGIMPI_FILE_WRITE_AT yogimpi_file_write_at_
#define YOGIMPI_INFO_CREATE yogimpi_info_create_
#define YOGIMPI_INFO_SET yogimpi_info_set_

static YogiMPI_Status* status_to_c(int *status) {
	return (YogiMPI_Status *)status;
}

void YOGIMPI_SEND(void *buffer, int *count, int *datatype, int *dest, int *tag,
		          int *comm, int *ierror) {
    *ierror = YogiMPI_Send(buffer, *count, *datatype, *dest, *tag, *comm);
}

void YOGIMPI_RECV(void *buffer, int *count, int *datatype, int *source, 
		          int *tag, int *comm, int *status, int *ierror) {
    *ierror = YogiMPI_Recv(buffer, *count, *datatype, *source, *tag, *comm,
		                   status_to_c(status));
}

void YOGIMPI_GET_COUNT(int *status, int *datatype, int *count, int *ierror)
{
    *ierror = YogiMPI_Get_count(status_to_c(status), *datatype, count);
}

void YOGIMPI_SSEND(void *buffer, int *count, int *datatype, int *dest, int *tag,
		           int *comm, int *ierror) {
      *ierror = YogiMPI_Ssend(buffer, *count, *datatype, *dest, *tag, *comm);
}

void YOGIMPI_ISEND(void *buffer, int *count, int *datatype, int *dest, int *tag,
		           int *comm, int *request, int *ierror) {
    *ierror = YogiMPI_Isend(buffer, *count, *datatype, *dest, *tag, *comm,
		                    request);
}

void YOGIMPI_ISSEND(void* buf, int *count, int *datatype, int *dest, 
		            int *tag, int *comm,  int *request, int *ierror) {
    *ierror = YogiMPI_Issend(buf, *count, *datatype, *dest, *tag, *comm,
		                     request);
}

void YOGIMPI_IRECV(void *buffer, int *count, int *datatype, int *source, 
		           int *tag, int *comm, int *request, int *ierror) {
    *ierror = YogiMPI_Irecv(buffer, *count, *datatype, *source, *tag, *comm,
		                    request);
}

void YOGIMPI_WAIT(int *request, int *status, int *ierror) {
    *ierror = YogiMPI_Wait(request, status_to_c(status));
}

void YOGIMPI_REQUEST_FREE(int *request, int *ierror) {
    *ierror = YogiMPI_Request_free(request);
}

void YOGIMPI_BARRIER(int *comm, int *ierror) {
    *ierror = YogiMPI_Barrier(*comm);
}

void YOGIMPI_BCAST(void *buffer, int *count, int *datatype, int *root, 
		           int *comm, int *ierror) {
    *ierror = YogiMPI_Bcast(buffer, *count, *datatype, *root, *comm);
}

void YOGIMPI_GATHER(void *sendbuf, int *sendcount, int *sendtype, void *recvbuf,
		            int *recvcount, int *recvtype, int *root, int *comm, 
					int *ierror) {
    *ierror = YogiMPI_Gather(sendbuf, *sendcount, *sendtype, recvbuf, 
    		                 *recvcount, *recvtype, *root, *comm);
}

void YOGIMPI_REDUCE(void *sendbuf, void *recvbuf, int *count, int *datatype, 
		            int *op, int *root, int *comm, int *ierror) {
    *ierror = YogiMPI_Reduce(sendbuf, recvbuf, *count, *datatype, *op, *root, 
    		                 *comm);
}

void YOGIMPI_ALLREDUCE(void *sendbuf, void *recvbuf, int *count, int *datatype, 
		               int *op, int *comm, int *ierror) {
    *ierror = YogiMPI_Allreduce(sendbuf, recvbuf, *count, *datatype, *op,
    		                    *comm);
}

void YOGIMPI_COMM_SIZE(int *comm, int *size, int *ierror) {
    *ierror = YogiMPI_Comm_size(*comm, size);
}

void YOGIMPI_COMM_RANK(int *comm, int *rank, int *ierror) {
    *ierror = YogiMPI_Comm_rank(*comm, rank);
}

void YOGIMPI_COMM_DUP(int *comm, int *newcomm, int *ierror) {
    *ierror = YogiMPI_Comm_dup(*comm, newcomm);
}

void YOGIMPI_COMM_SPLIT(int *comm, int *color, int *key, int *newcomm, 
		                int *ierror) {
    *ierror = YogiMPI_Comm_split(*comm, *color, *key, newcomm);
}

void YOGIMPI_COMM_FREE(int *comm, int *ierror) {
    *ierror = YogiMPI_Comm_free(comm);
}

void YOGIMPI_GET_PROCESSOR_NAME(char *name, int *resultlen, int *ierror) {
    *ierror = YogiMPI_Get_processor_name(name, resultlen); 
}

double YOGIMPI_WTIME() {
    return YogiMPI_Wtime();
}

void YOGIMPI_INIT(int *ierror) 
{
    *ierror = YogiMPI_Init(NULL, NULL);
}

void YOGIMPI_FINALIZE(int *ierror) 
{
  *ierror = YogiMPI_Finalize();
}

void YOGIMPI_TYPE_INDEXED(int *count, int *array_of_blocklengths, 
                          int *array_of_displacements, int *oldtype,
		                  int *newtype, int *ierror) {
		
	*ierror = YogiMPI_Type_indexed(*count, array_of_blocklengths, 
		                           array_of_displacements, *oldtype, newtype);						 
}

void YOGIMPI_TYPE_VECTOR(int *count, int *blocklength, int *stride, 
                         int *oldtype, int *newtype, int *ierror) {
		
	*ierror = YogiMPI_Type_vector(*count, *blocklength, *stride, *oldtype,
			                      newtype);
}

void YOGIMPI_TYPE_CONTIGUOUS(int *count, int *oldtype, int *newtype, 
		                     int *ierror) {
	*ierror = YogiMPI_Type_contiguous(*count, *oldtype, newtype);				 
}

void YOGIMPI_TYPE_COMMIT(YogiMPI_Datatype *datatype, int *ierror) {
	*ierror = YogiMPI_Type_commit(datatype);
}

void YOGIMPI_TYPE_FREE(int *datatype, int *ierror) {
	*ierror = YogiMPI_Type_free(datatype);
}

void YOGIMPI_TYPE_SIZE(int *datatype, int *size, int *ierror) {
	*ierror = YogiMPI_Type_size(*datatype, size);
}

void YOGIMPI_TYPE_CREATE_INDEXED_BLOCK(int *count, int *blocklength, 
                                       int *array_of_displacements,
                                       int *oldtype, int *newtype,
									   int *ierror) {
    *ierror = YogiMPI_Type_create_indexed_block(*count, *blocklength, 
                                                array_of_displacements,
                                                *oldtype, newtype);
}

void YOGIMPI_RECV_INIT(void *buf, int *count, int *datatype, int *source, 
		               int *tag, int *comm, int *request, int *ierror) {
    *ierror = YogiMPI_Recv_init(buf, *count, *datatype, *source, *tag, *comm,
			                     request);
}

void YOGIMPI_SCAN(void *sendbuf, void *recvbuf, int *count, int *datatype,
		          int *op, int *comm, int *ierror) {
    *ierror = YogiMPI_Scan(sendbuf, recvbuf, *count, *datatype, *op, *comm);
}

void YOGIMPI_STARTALL(int *count, int *array_of_requests, int *ierror) {
    *ierror = YogiMPI_Startall(*count, array_of_requests);
}

void YOGIMPI_ALLTOALL(void *sendbuf, int *sendcount, int *sendtype, 
		              void *recvbuf, int *recvcount, int *recvtype, int *comm,
					  int *ierror) {
	*ierror = YogiMPI_Alltoall(sendbuf, *sendcount, *sendtype, recvbuf, 
			                   *recvcount, *recvtype, *comm);
}

void YOGIMPI_ALLTOALLV(void *sendbuf, int *sendcounts, int *sdispls, 
		               int *sendtype, void *recvbuf, int *recvcounts,
					   int *rdispls, int *recvtype, int *comm, int *ierror) {
    *ierror = YogiMPI_Alltoallv(sendbuf, sendcounts, sdispls, *sendtype, 
						        recvbuf, recvcounts, rdispls, *recvtype, *comm);
}

void YOGIMPI_FILE_CLOSE(int *fh, int *ierror) {
	*ierror = YogiMPI_File_close(fh);
}

void YOGIMPI_FILE_GET_INFO(int *fh, int *info_used, int *ierror) {
	*ierror = YogiMPI_File_get_info(*fh, info_used);
}

void YOGIMPI_FILE_OPEN(int *comm, char *filename, int *amode, int *info, 
		               int *fh, int *ierror) {
	*ierror = YogiMPI_File_open(*comm, filename, *amode, *info, fh);
}

void YOGIMPI_FILE_SET_VIEW(int *fh, long long int *disp, int *etype, 
		                   int *filetype, char *datarep, int *info,
						   int *ierror) { 
    *ierror = YogiMPI_File_set_view(*fh, *disp, *etype, *filetype, datarep,
    		                        *info);
}

void YOGIMPI_FILE_WRITE_ALL(int *fh, void *buf, int *count, int *datatype, 
		                    int *status, int *ierror) {
	*ierror = YogiMPI_File_write_all(*fh, buf, *count, *datatype, 
			                         status_to_c(status));
}


void YOGIMPI_FILE_WRITE_AT(int *fh, long long int *offset, void *buf, 
		                   int *count, int *datatype, int *status,
						   int *ierror) {
	*ierror = YogiMPI_File_write_at(*fh, *offset, buf, *count, *datatype, 
						            status_to_c(status));
}

void YOGIMPI_INFO_CREATE(int *info, int *ierror) {
	*ierror = YogiMPI_Info_create(info);
}

void YOGIMPI_INFO_SET(int *info, char *key, char *value, int *ierror) {
    *ierror = YogiMPI_Info_set(*info, key, value);
}
