#include <yogimpi.h>

int YogiMPI_Send(void* buf, int count, YogiMPI_Datatype datatype, int dest,
                 int tag, YogiMPI_Comm comm) {

}

int YogiMPI_Recv(void* buf, int count, YogiMPI_Datatype datatype, int source,
                 int tag, YogiMPI_Comm comm, YogiMPI_Status *status) {

}

int YogiMPI_Get_count(YogiMPI_Status *status, YogiMPI_Datatype datatype, 
                      int *count) {

}

int YogiMPI_Ssend(void* buf, int count, YogiMPI_Datatype datatype, int dest,
                  int tag, YogiMPI_Comm comm) {

}

int YogiMPI_Isend(void *buf, int count, YogiMPI_Datatype datatype, int dest,
                  int tag, YogiMPI_Comm comm, YogiMPI_Request* request) {

}

int YogiMPI_Issend(void *buf, int count, YogiMPI_Datatype datatype, int dest,
                   int tag, YogiMPI_Comm comm, YogiMPI_Request* request) {

}

int YogiMPI_Irecv(void *buf, int count, YogiMPI_Datatype datatype, int source,
                  int tag, YogiMPI_Comm comm, YogiMPI_Request *request) {

}

int YogiMPI_Wait(YogiMPI_Request* request, YogiMPI_Status* status) {

}

int YogiMPI_Request_free(YogiMPI_Request *request) {

}

int YogiMPI_Waitall(int count, YogiMPI_Request *array_of_requests,
                    YogiMPI_Status *array_of_statuses) {

}

int YogiMPI_Send_init(void *buf, int count, YogiMPI_Datatype datatype, int dest,
                      int tag, YogiMPI_Comm comm, YogiMPI_Request* request) {

}

int YogiMPI_Type_size(const YogiMPI_Datatype datatype, int * size) {

}

int YogiMPI_Type_contiguous(int count, YogiMPI_Datatype oldtype, 
                            YogiMPI_Datatype *newtype) {

}

int YogiMPI_Type_vector(int count, int blocklength, int stride, 
                        YogiMPI_Datatype oldtype, YogiMPI_Datatype *newtype) {

}

int YogiMPI_Type_indexed(int count, int *array_of_blocklengths, 
                         int *array_of_displacements, YogiMPI_Datatype oldtype,
                         YogiMPI_Datatype *newtype) {

}

int YogiMPI_Type_commit(YogiMPI_Datatype *datatype) {

}

int YogiMPI_Type_free(YogiMPI_Datatype *datatype) {

}

int YogiMPI_Barrier(YogiMPI_Comm comm) {

}

int YogiMPI_Bcast(void* buffer, int count, YogiMPI_Datatype datatype, int root,
                  YogiMPI_Comm comm) {

}

int YogiMPI_Gather(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype,
                   void* recvbuf, int recvcount, YogiMPI_Datatype recvtype,
                   int root, YogiMPI_Comm comm) {

}

int YogiMPI_Gatherv(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype, 
                    void* recvbuf, int *recvcounts, int *displs, 
                    YogiMPI_Datatype recvtype, int root, YogiMPI_Comm comm) {

}

int YogiMPI_Scatterv(void* sendbuf, int *sendcounts, int *displs, 
                     YogiMPI_Datatype sendtype, void* recvbuf, int recvcount,
                     YogiMPI_Datatype recvtype, int root, YogiMPI_Comm comm) {

}

int YogiMPI_Allgather(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype,
                      void* recvbuf, int recvcount, YogiMPI_Datatype recvtype,
                      YogiMPI_Comm comm) {

}

int YogiMPI_Allgatherv(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype,
                       void* recvbuf, int* recvcounts, int* displs, 
                       YogiMPI_Datatype recvtype, YogiMPI_Comm comm) {

}

int YogiMPI_Reduce(void* sendbuf, void* recvbuf, int count, 
                   YogiMPI_Datatype datatype, YogiMPI_Op op, int root,
                   YogiMPI_Comm comm) {

}

int YogiMPI_Allreduce(void* sendbuf, void* recvbuf, int count,
                      YogiMPI_Datatype datatype, YogiMPI_Op op,
                      YogiMPI_Comm comm) {

}

int YogiMPI_Comm_size(YogiMPI_Comm comm, int* size) {

}

int YogiMPI_Comm_rank(YogiMPI_Comm comm, int* rank) {

}

int YogiMPI_Comm_dup(YogiMPI_Comm comm, YogiMPI_Comm* out) {

}

int YogiMPI_Comm_split(YogiMPI_Comm comm, int color, int key,
                       YogiMPI_Comm* new_comm) {

}

int YogiMPI_Comm_free(YogiMPI_Comm *comm) {

}

int YogiMPI_Get_processor_name(char *name, int *resultlen) {

}

double YogiMPI_Wtime() {

}

int YogiMPI_Init(int* argc, char ***argv) {

}

int YogiMPI_Finalize() {

}

/* New MPI 2.0 functions */

int YogiMPI_Type_create_indexed_block(int count, int blocklength, 
                                      int array_of_displacements[],
                                      YogiMPI_Datatype oldtype,
                                      YogiMPI_Datatype *newtype) {

}

YogiMPI_Comm YogiMPI_Comm_f2c(YogiMPI_Fint comm) {

}
