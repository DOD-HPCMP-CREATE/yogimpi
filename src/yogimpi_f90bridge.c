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

/* File to bind YogiMPI C functions to Fortran. */

#include "yogimpi.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/* Access common block data from Fortran to determine the address of the
   MPI_STATUS_IGNORE and MPI_STATUSES_IGNORE. */
extern struct {
  YogiMPI_Status fignore;
} ympiscalar_; 

extern struct {
  YogiMPI_Status fignore;
} ympiarray_; 

/* Compares an integer pointer from Fortran to those defined in common blocks
   for YogiMPI_STATUS_IGNORE and YogiMPI_STATUSES_IGNORE in Fortran.  Returns
   true if either match pointer address, otherwise returns false. */
static int check_to_ignore(int *raw_status) {
    if ((void *)raw_status == (void *)&(ympiscalar_.fignore)) return 1;
    if ((void *)raw_status == (void *)&(ympiarray_.fignore)) return 1;
    return 0;
}

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
#define YOGIMPI_FILE_READ_AT yogimpi_file_read_at_
#define YOGIMPI_FILE_READ_ALL yogimpi_file_read_all_
#define YOGIMPI_INFO_CREATE yogimpi_info_create_
#define YOGIMPI_INFO_SET yogimpi_info_set_
#define YOGIMPI_INFO_FREE yogimpi_info_free_
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
#define YOGIMPI_IPROBE f90bridge_yogimpi_iprobe_
#define YOGIMPI_TEST f90bridge_yogimpi_test_
#define YOGIMPI_ATTR_GET f90bridge_yogimpi_attr_get_
#define YOGIMPI_INITIALIZED f90bridge_yogimpi_initialized_
#define YOGIMPI_FINALIZED f90bridge_yogimpi_finalized_
#define YOGIMPI_BSEND yogimpi_bsend_
#define YOGIMPI_BUFFER_ATTACH yogimpi_buffer_attach_
#define YOGIMPI_FILE_DELETE yogimpi_file_delete_
#define YOGIMPI_FILE_WRITE yogimpi_file_write_
#define YOGIMPI_FILE_SEEK yogimpi_file_seek_

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
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_Recv(buffer, *count, *datatype, *source, *tag, *comm,
        	               YogiMPI_STATUS_IGNORE);	
    }
    else {
        *ierror = YogiMPI_Recv(buffer, *count, *datatype, *source, *tag, *comm,
    	                       (YogiMPI_Status *)status);
    }
}

void YOGIMPI_GET_COUNT(int *status, int *datatype, int *count, int *ierror)
{
    /* MPI_Status argument can't be ignored. */
    assert(check_to_ignore(status) == 0);
    *ierror = YogiMPI_Get_count((YogiMPI_Status *)status, *datatype, count);
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
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_Wait(request, YogiMPI_STATUS_IGNORE);
    }
    else {
        *ierror = YogiMPI_Wait(request, (YogiMPI_Status *)status);
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
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_File_write_all(*fh, buf, *count, *datatype, 
                                         YogiMPI_STATUS_IGNORE);		
    }
    else {
        *ierror = YogiMPI_File_write_all(*fh, buf, *count, *datatype,
                                         (YogiMPI_Status *)status);
    }
}


void YOGIMPI_FILE_WRITE_AT(int *fh, long long int *offset, void *buf, 
                           int *count, int *datatype, int *status,
                           int *ierror) {
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_File_write_at(*fh, *offset, buf, *count, *datatype, 
                                        YogiMPI_STATUS_IGNORE);		
    }
    else {
        *ierror = YogiMPI_File_write_at(*fh, *offset, buf, *count, *datatype, 
                                        (YogiMPI_Status *)status);
    }
}

void YOGIMPI_FILE_READ_AT(int *fh, long long int *offset, void *buf,
                          int *count, int *datatype, int *status,
                          int *ierror) {
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_File_read_at(*fh, *offset, buf, *count, *datatype,
                                       YogiMPI_STATUS_IGNORE);
    }
    else {
        *ierror = YogiMPI_File_read_at(*fh, *offset, buf, *count, *datatype,
                                       (YogiMPI_Status *)status);
    }
}

void YOGIMPI_FILE_READ_ALL(int *fh, void *buf, int *count, int *datatype,
                           int *status, int *ierror) {
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_File_read_all(*fh, buf, *count, *datatype,
                                        YogiMPI_STATUS_IGNORE);
    }
    else {
        *ierror = YogiMPI_File_read_all(*fh, buf, *count, *datatype,
                                        (YogiMPI_Status *)status);
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

void YOGIMPI_INFO_FREE(int *info, int *ierror) {
    *ierror = YogiMPI_Info_free(info);
}

void YOGIMPI_WAITALL(int *count, int *array_of_requests, int *array_of_statuses,
                     int *ierror) {
    if (check_to_ignore(array_of_statuses)) {
        *ierror = YogiMPI_Waitall(*count, array_of_requests,
                                  YogiMPI_STATUSES_IGNORE);
    }
    else {
        *ierror = YogiMPI_Waitall(*count, array_of_requests,
                                  (YogiMPI_Status *)array_of_statuses);
    }
}

void YOGIMPI_WAITANY(int *count, int *array_of_requests, int *index,
                     int *status, int *ierror) {
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_Waitany(*count, array_of_requests, index, 
                                  YogiMPI_STATUS_IGNORE);
    }
    else {
        *ierror = YogiMPI_Waitany(*count, array_of_requests, index,
                                  (YogiMPI_Status *)status);
    }
}

void YOGIMPI_WAITSOME(int *incount, int *array_of_requests, int *outcount,
                      int *array_of_indices, int *array_of_statuses,
                      int *ierror) {
    if (check_to_ignore(array_of_statuses)) {
        *ierror = YogiMPI_Waitsome(*incount, array_of_requests, outcount, 
                                   array_of_indices, YogiMPI_STATUSES_IGNORE);
    }
    else {
        *ierror = YogiMPI_Waitsome(*incount, array_of_requests, outcount, 
                                   array_of_indices,
                                   (YogiMPI_Status *)array_of_statuses);
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
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_Test(request, flag, YogiMPI_STATUS_IGNORE);
    }
    else {
        *ierror = YogiMPI_Test(request, flag, (YogiMPI_Status *)status);
    }
}

void YOGIMPI_PROBE(int *source, int *tag, int *comm, int *status, int *ierror) {
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_Probe(*source, *tag, *comm, YogiMPI_STATUS_IGNORE);
    }
    else {
        *ierror = YogiMPI_Probe(*source, *tag, *comm, (YogiMPI_Status *)status);
    }
}

void YOGIMPI_IPROBE(int *source, int *tag, int *comm, int *flag, int *status,
                    int *ierror) {
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_Iprobe(*source, *tag, *comm, flag, 
                                 YogiMPI_STATUS_IGNORE);
    }
    else {
        *ierror = YogiMPI_Iprobe(*source, *tag, *comm, flag,
                                 (YogiMPI_Status *)status);
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

void YOGIMPI_BSEND(int *buf, int *count, int *datatype, int *dest,
                   int *tag, int *comm, int *ierror) {
    *ierror = YogiMPI_Bsend(buf, *count, *datatype, *dest, *tag, *comm);
}

void YOGIMPI_BUFFER_ATTACH(int *buf, int *size, int *ierror) {
    *ierror = YogiMPI_Buffer_attach(buf, *size);
}

void YOGIMPI_FILE_DELETE(char* filename, int *info, int *ierror,
                         int filename_len) {
    char *interimName = null_terminate(filename, filename_len);
    *ierror = YogiMPI_File_delete(interimName, *info);
}

void YOGIMPI_FILE_WRITE(int *fh, int *buf, int *count, int *datatype, 
                        int *status, int *ierror) {
	
    if (check_to_ignore(status)) {
        *ierror = YogiMPI_File_write(*fh, buf, *count, *datatype, 
                                     YogiMPI_STATUS_IGNORE);
    }
    else {
        *ierror = YogiMPI_File_write(*fh, buf, *count, *datatype,
                                     (YogiMPI_Status *)status);
    }
}

void YOGIMPI_FILE_SEEK(int *fh, YogiMPI_Offset *offset, int *whence, 
                       int *ierror) {
    *ierror = YogiMPI_File_seek(*fh, *offset, *whence);
}

