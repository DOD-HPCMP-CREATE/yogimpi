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
#define YOGIMPI_IRECV yogimpi_irecv_
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

void YogiMPI_ISEND(void *buffer, int *count, int *datatype, int *dest, int *tag,
		           int *comm, int *request, int *ierror) {
    *ierror = YogiMPI_Isend(buffer, *count, *datatype, *dest, *tag, *comm,
		                    request);
}

void YogiMPI_IRECV(void *buffer, int *count, int *datatype, int *source, 
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
