/* File to bind YogiMPI C functions to Fortran. */

#include "yogimpi.h"
#include "mpi.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/* What the Fortran value is for ignoring status. */
#define FYOGIMPI_STATUS_IGNORE -1
/* What the Fortran value is for ignoring all statuses. */
#define FYOGIMPI_STATUSES_IGNORE -1

/* Define how the C functions will appear to Fortran.  Typically this is all
 * lowercase, with an additional underscore at the end.
 */
#define YOGIMPI_SEND yogimpi_send_
#define YOGIMPI_RECV yogimpi_recv_
#define YOGIMPI_RECV_INIT yogimpi_recv_init_
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
#define YOGIMPI_COMM_CREATE yogimpi_comm_create_
#define YOGIMPI_COMM_GROUP yogimpi_comm_group_
#define YOGIMPI_COMM_SIZE yogimpi_comm_size_
#define YOGIMPI_COMM_RANK yogimpi_comm_rank_
#define YOGIMPI_COMM_DUP yogimpi_comm_dup_
#define YOGIMPI_COMM_SPLIT yogimpi_comm_split_
#define YOGIMPI_COMM_FREE yogimpi_comm_free_
#define YOGIMPI_GROUP_FREE yogimpi_group_free_
#define YOGIMPI_GROUP_INCL yogimpi_group_incl_
#define YOGIMPI_GROUP_RANK yogimpi_group_rank_
#define YOGIMPI_GROUP_TRANSLATE_RANKS yogimpi_group_translate_ranks_
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
#define YOGIMPI_WAITANY yogimpi_waitany_
#define YOGIMPI_WAITSOME yogimpi_waitsome_
#define YOGIMPI_WAITALL yogimpi_waitall_
#define YOGIMPI_SEND_INIT yogimpi_send_init_
#define YOGIMPI_GATHERV yogimpi_gatherv_
#define YOGIMPI_SCATTER yogimpi_scatter_
#define YOGIMPI_SCATTERV yogimpi_scatterv_
#define YOGIMPI_ALLGATHER yogimpi_allgather_
#define YOGIMPI_ALLGATHERV yogimpi_allgatherv_
#define YOGIMPI_ABORT yogimpi_abort_
#define YOGIMPI_PROBE yogimpi_probe_
#define YOGIMPI_IPROBE yogimpi_iprobe_
#define YOGIMPI_TEST yogimpi_test_
#define YOGIMPI_ATTR_GET yogimpi_attr_get_
#define YOGIMPI_INITIALIZED yogimpi_initialized_
#define YOGIMPI_FINALIZED yogimpi_finalized_

/* Returns whether YogiMPI's Fortran layer should place MPI_STATUS_IGNORE and
 * MPI_STATUSES_IGNORE as arguments when MPI_Status(es) are expected.  This
 * overrides any choice the user makes.
 */
static int get_fignore_status() {
    if (getenv("YMPI_FSTATUS_IGNORE") != NULL) return 1;
    return 0;
}

static void yogi_fstatus_to_c(int *source, YogiMPI_Status *dest)
{
	memcpy((void *)dest->realStatus, (void *)source, sizeof(MPI_Status));
}

static void yogi_cstatus_to_f(YogiMPI_Status *source, int *dest) {
	memcpy((void *)dest, (void *)source->realStatus, sizeof(MPI_Status));
}

static YogiMPI_Status * yogi_fstatus_array_to_c(int *status, int count) {
	YogiMPI_Status *list = (YogiMPI_Status*)malloc(count*sizeof(YogiMPI_Status));
	int i;
	for (i = 0; i < count; i++) {
		int offset = i * MAX_STATUS_SIZE;
        yogi_fstatus_to_c(&status[offset], &list[i]);
	}
	return list;
}

static void yogi_cstatus_array_to_f(YogiMPI_Status *list, int *status,
		                            int count) {
	int i;
	for (i = 0; i < count; i++) {
		int offset = i * MAX_STATUS_SIZE;
        yogi_cstatus_to_f(&list[i], &status[offset]);
	}
}

static char * null_terminate(char *fString, size_t slen) {
    char * returnString = (char *)malloc(slen + 1);
    strncpy(returnString, fString, slen);
    returnString[slen] = '\0';
    return returnString;
}		

void YOGIMPI_SEND(void *buffer, int *count, int *datatype, int *dest, int *tag,
		          int *comm, int *ierror) {
    *ierror = YogiMPI_Send(buffer, *count, *datatype, *dest, *tag, *comm);
}

void YOGIMPI_RECV(void *buffer, int *count, int *datatype, int *source, 
		          int *tag, int *comm, int *status, int *ierror) {
    if (*status == FYOGIMPI_STATUS_IGNORE || get_fignore_status()) {
        *ierror = YogiMPI_Recv(buffer, *count, *datatype, *source, *tag, *comm,
        	                   YogiMPI_STATUS_IGNORE);	
    }
	else {
		YogiMPI_Status cStatus;
		yogi_fstatus_to_c(status, &cStatus);
        *ierror = YogiMPI_Recv(buffer, *count, *datatype, *source, *tag, *comm,
    	                       &cStatus);
        yogi_cstatus_to_f(&cStatus, status);
    }
}

void YOGIMPI_GET_COUNT(int *status, int *datatype, int *count, int *ierror)
{
	/* No overriding the MPI_Status argument - it cannot be ignored. */
	assert(get_fignore_status() == 0);
	YogiMPI_Status cStatus;
	yogi_fstatus_to_c(status, &cStatus);
    *ierror = YogiMPI_Get_count(&cStatus, *datatype, count);
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
	if (*status == FYOGIMPI_STATUS_IGNORE || get_fignore_status()) {
        *ierror = YogiMPI_Wait(request, YogiMPI_STATUS_IGNORE);
	}
	else {
		YogiMPI_Status cStatus;
		yogi_fstatus_to_c(status, &cStatus);
        *ierror = YogiMPI_Wait(request, &cStatus);
        yogi_cstatus_to_f(&cStatus, status);
	}        

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

void YOGIMPI_COMM_CREATE(int *comm, int *group, int *newcomm, int *ierror) {
    *ierror = YogiMPI_Comm_create(*comm, *group, newcomm);
}

void YOGIMPI_COMM_GROUP(int *comm, int *group, int *ierror) {
    *ierror = YogiMPI_Comm_group(*comm, group); 
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

void YOGIMPI_GROUP_FREE(int *group, int *ierror) {
    *ierror = YogiMPI_Group_free(group);
}

void YOGIMPI_GROUP_INCL(int *group, int *n, int *ranks, int *group_out,
		                int *ierror) {
	*ierror = YogiMPI_Group_incl(*group, *n, ranks, group_out);
}

void YOGIMPI_GROUP_TRANSLATE_RANKS(int *group1, int *n, int *ranks1,
		                           int *group2, int *ranks2, int *ierror) {
	*ierror = YogiMPI_Group_translate_ranks(*group1, *n, ranks1, *group2, 
			                                ranks2);
}

void YOGIMPI_GROUP_RANK(int *group, int *rank, int *ierror) {
    *ierror = YogiMPI_Group_rank(*group, rank);
}

void YOGIMPI_GET_PROCESSOR_NAME(char *name, int *resultlen, int *ierror,
		                        int name_len) {
	char *interimName = null_terminate(name, name_len);
    *ierror = YogiMPI_Get_processor_name(interimName, resultlen);
    if (*resultlen > name_len) *resultlen = name_len;
	strncpy(name, interimName, name_len);
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
		               int *fh, int *ierror, int filename_len) {
	*ierror = YogiMPI_File_open(*comm, null_terminate(filename, filename_len),
			                    *amode, *info, fh);
}

void YOGIMPI_FILE_SET_VIEW(int *fh, long long int *disp, int *etype, 
		                   int *filetype, char *datarep, int *info,
						   int *ierror, int datarep_len) { 
    *ierror = YogiMPI_File_set_view(*fh, *disp, *etype, *filetype, 
    		                        null_terminate(datarep, datarep_len),
    		                        *info);
}

void YOGIMPI_FILE_WRITE_ALL(int *fh, void *buf, int *count, int *datatype, 
		                    int *status, int *ierror) {
	if (*status == FYOGIMPI_STATUS_IGNORE || get_fignore_status()) {
	    *ierror = YogiMPI_File_write_all(*fh, buf, *count, *datatype, 
			                             YogiMPI_STATUS_IGNORE);		
	}
	else {
		YogiMPI_Status cStatus;
		yogi_fstatus_to_c(status, &cStatus);
	    *ierror = YogiMPI_File_write_all(*fh, buf, *count, *datatype, 
			                             &cStatus);
	    yogi_cstatus_to_f(&cStatus, status);
	}
}


void YOGIMPI_FILE_WRITE_AT(int *fh, long long int *offset, void *buf, 
		                   int *count, int *datatype, int *status,
						   int *ierror) {
	if (*status == FYOGIMPI_STATUS_IGNORE || get_fignore_status()) {
	    *ierror = YogiMPI_File_write_at(*fh, *offset, buf, *count, *datatype, 
						                YogiMPI_STATUS_IGNORE);		
	}
	else {
		YogiMPI_Status cStatus;
		yogi_fstatus_to_c(status, &cStatus);
	    *ierror = YogiMPI_File_write_at(*fh, *offset, buf, *count, *datatype, 
						                &cStatus);
	    yogi_cstatus_to_f(&cStatus, status);
	}
}

void YOGIMPI_INFO_CREATE(int *info, int *ierror) {
	*ierror = YogiMPI_Info_create(info);
}

void YOGIMPI_INFO_SET(int *info, char *key, char *value, int *ierror, 
		              int key_len, int value_len) {
    *ierror = YogiMPI_Info_set(*info, null_terminate(key, key_len),
    		                   null_terminate(value, value_len));
}

void YOGIMPI_WAITALL(int *count, int *array_of_requests, int *array_of_statuses,
		             int *ierror) {
	if (*array_of_statuses == FYOGIMPI_STATUSES_IGNORE) {
	    *ierror = YogiMPI_Waitall(*count, array_of_requests, 
	    		                  YogiMPI_STATUSES_IGNORE);
	}
	else {
	    YogiMPI_Status *list = yogi_fstatus_array_to_c(array_of_statuses, 
	    		                                       *count);
	    *ierror = YogiMPI_Waitall(*count, array_of_requests, list);
	    yogi_cstatus_array_to_f(list, array_of_statuses, *count);
	    free(list);
	}
}

void YOGIMPI_WAITANY(int *count, int *array_of_requests, int *index,
                     int *status, int *ierror) {
    if (*status == FYOGIMPI_STATUS_IGNORE || get_fignore_status()) {
        *ierror = YogiMPI_Waitany(*count, array_of_requests, index, 
                                  YogiMPI_STATUS_IGNORE);
    }
    else {
        YogiMPI_Status cStatus;
        yogi_fstatus_to_c(status, &cStatus);
        *ierror = YogiMPI_Waitany(*count, array_of_requests, index, &cStatus);
        yogi_cstatus_to_f(&cStatus, status);
    }
}

void YOGIMPI_WAITSOME(int *incount, int *array_of_requests, int *outcount,
                      int *array_of_indices, int *array_of_statuses,
                      int *ierror) {
    if (*array_of_statuses == FYOGIMPI_STATUSES_IGNORE 
        || get_fignore_status()) {
        *ierror = YogiMPI_Waitsome(*incount, array_of_requests, outcount, 
                                   array_of_indices, YogiMPI_STATUSES_IGNORE);
    }
    else {
        YogiMPI_Status *list = yogi_fstatus_array_to_c(array_of_statuses, 
                                                       *incount);
        *ierror = YogiMPI_Waitsome(*incount, array_of_requests, outcount, 
                                   array_of_indices, list);
        yogi_cstatus_array_to_f(list, array_of_statuses, *outcount);
        free(list);
    }
}

void YOGIMPI_SEND_INIT(void *buf, int *count, int *datatype, int *dest,
		               int *tag, int *comm, int *request, int *ierror) {
    *ierror = YogiMPI_Send_init(buf, *count, *datatype, *dest, *tag, *comm,
    		                    request);
}

void YOGIMPI_GATHERV(void *sendbuf, int *sendcount, int *sendtype, 
		             void *recvbuf, int *recvcounts, int *displs,
					 int *recvtype, int *root, int *comm, int *ierror) {
	*ierror = YogiMPI_Gatherv(sendbuf, *sendcount, *sendtype, recvbuf, 
			                  recvcounts, displs, *recvtype, *root, *comm);
}

void YOGIMPI_SCATTER(void *sendbuf, int *sendcount, int *sendtype,
                     void *recvbuf, int *recvcount, int *recvtype, 
					 int *root, int *comm, int *ierror) {
	*ierror = YogiMPI_Scatter(sendbuf, *sendcount, *sendtype, recvbuf, 
			                  *recvcount, *recvtype, *root, *comm);
}

void YOGIMPI_SCATTERV(void *sendbuf, int *sendcounts, int *displs, 
                      int *sendtype, void *recvbuf, int *recvcount,
		              int *recvtype, int *root, int *comm, int *ierror) {
    *ierror = YogiMPI_Scatterv(sendbuf, sendcounts, displs, *sendtype, recvbuf,
    		                   *recvcount, *recvtype, *root, *comm);
}

void YOGIMPI_ALLGATHER(void *sendbuf, int *sendcount, int *sendtype, 
                       void* recvbuf, int *recvcount, int *recvtype, 
		               int *comm, int *ierror) {
	*ierror = YogiMPI_Allgather(sendbuf, *sendcount, *sendtype, recvbuf, 
			                    *recvcount, *recvtype, *comm);
}

void YOGIMPI_ALLGATHERV(void *sendbuf, int *sendcount, int *sendtype, 
                        void *recvbuf, int *recvcounts, int *displs, 
                        int *recvtype, int *comm, int *ierror) {
    *ierror = YogiMPI_Allgatherv(sendbuf, *sendcount, *sendtype, recvbuf, 
                                 recvcounts, displs, *recvtype, *comm);

}

void YOGIMPI_TEST(int *request, int *flag, int *status, int *ierror) {
    if (*status == FYOGIMPI_STATUS_IGNORE || get_fignore_status()) {
        *ierror = YogiMPI_Test(request, flag, YogiMPI_STATUS_IGNORE);
    }
    else {
        YogiMPI_Status cStatus;
        yogi_fstatus_to_c(status, &cStatus);
        *ierror = YogiMPI_Test(request, flag, &cStatus);
        yogi_cstatus_to_f(&cStatus, status);
    }

}

void YOGIMPI_PROBE(int *source, int *tag, int *comm, int *status, int *ierror) {
    if (*status == FYOGIMPI_STATUS_IGNORE || get_fignore_status()) {
        *ierror = YogiMPI_Probe(*source, *tag, *comm, YogiMPI_STATUS_IGNORE);
    }
    else {
        YogiMPI_Status cStatus;
        yogi_fstatus_to_c(status, &cStatus);
        *ierror = YogiMPI_Probe(*source, *tag, *comm, &cStatus);
        yogi_cstatus_to_f(&cStatus, status);
    }

}

void YOGIMPI_IPROBE(int *source, int *tag, int *comm, int *flag, int *status,
                    int *ierror) {
    if (*status == FYOGIMPI_STATUS_IGNORE || get_fignore_status()) {
        *ierror = YogiMPI_Iprobe(*source, *tag, *comm, flag, 
                                 YogiMPI_STATUS_IGNORE);
    }
    else {
        YogiMPI_Status cStatus;
        yogi_fstatus_to_c(status, &cStatus);
        *ierror = YogiMPI_Iprobe(*source, *tag, *comm, flag, &cStatus);
        yogi_cstatus_to_f(&cStatus, status);
    }
}

void YOGIMPI_ABORT(int *comm, int *errorcode, int *ierror) {
    *ierror = YogiMPI_Abort(*comm, *errorcode);
}

void YOGIMPI_ATTR_GET(int *comm, int *keyval, int *attribute_val,
                      int *flag, int *ierror) {
    *ierror = YogiMPI_Attr_get(*comm, *keyval, attribute_val, flag);
}

void YOGIMPI_INITIALIZED(int *flag, int *ierror) {
    *ierror = YogiMPI_Initialized(flag);
}

void YOGIMPI_FINALIZED(int *flag, int *ierror) {
    *ierror = YogiMPI_Finalized(flag);
}
