#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
  
  int pool_size, my_rank;
  int message_len;
  const int BUFFER_SIZE = 100;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &pool_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 
  char *testMessage = "This is a test message to be sent.";
  message_len = strlen(testMessage);

  if (my_rank == 0) {

    char send_buffer[BUFFER_SIZE];
    sprintf(send_buffer, testMessage);
    MPI_Request request;
    MPI_Status status;

    MPI_Isend(testMessage, message_len + 1, MPI_CHAR,
              1, 77, MPI_COMM_WORLD, &request);
    MPI_Wait (&request, &status);

    assert(request == MPI_REQUEST_NULL);

  } 
  else if (my_rank == 1) {

    char recv_buffer[BUFFER_SIZE];
    int count;
    MPI_Request request;
    MPI_Status status;
    
    MPI_Irecv(recv_buffer, message_len + 1, MPI_CHAR, 0, 77, MPI_COMM_WORLD,
              &request);
    MPI_Wait(&request, &status);
    assert(request == MPI_REQUEST_NULL);
    assert(strcmp(recv_buffer, testMessage) == 0);
    MPI_Get_count(&status, MPI_CHAR, &count);
    assert(count == message_len + 1);

  }

  MPI_Finalize();

}
