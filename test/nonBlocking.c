#include <stdio.h>
#include <string.h>
#include "mpitoyogi.h"

int main(int argc, char* argv[]) {
  
  int pool_size, my_rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &pool_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  if (my_rank == 0) {

    char send_buffer[BUFSIZ], my_cpu_name[BUFSIZ];
    int my_name_length;
    MPI_Request request;
    MPI_Status status;

    MPI_Get_processor_name(my_cpu_name, &my_name_length);
    sprintf (send_buffer, "Dear Task 1,\n\
Please do not send any more messages.\n\
Please send money instead.\n\
\tYours faithfully,\n\
\tTask 0\n\
\tRunning on %s\n", my_cpu_name);
    MPI_Isend (send_buffer, strlen(send_buffer) + 1, MPI_CHAR,
               1, 77, MPI_COMM_WORLD, &request);
    printf("hello there user, I've just started this send\n\
and I'm having a good time relaxing.\n");
    MPI_Wait (&request, &status);
    printf("hello there user, it looks like the message has been sent.\n");

    if (request == MPI_REQUEST_NULL) {
      printf("\tthe send request is MPI_REQUEST_NULL now\n");
    } else {
      printf("\tthe send request still lingers\n");
    }

  } 
  else if (my_rank == 1) {

    char recv_buffer[BUFSIZ], my_cpu_name[BUFSIZ];
    int my_name_length, count;
    MPI_Request request;
    MPI_Status status;
    
    MPI_Get_processor_name(my_cpu_name, &my_name_length);
    MPI_Irecv (recv_buffer, BUFSIZ, MPI_CHAR, 0, 77, MPI_COMM_WORLD,
               &request);
    printf("hello there user, I've just started this receive\n\
on %s, and I'm having a good time relaxing.\n", my_cpu_name);
    MPI_Wait(&request, &status);
    MPI_Get_count(&status, MPI_CHAR, &count);
    printf("hello there user, it looks like %d characters \
have just arrived:\n", count );
    printf("%s", recv_buffer);

    if (request == MPI_REQUEST_NULL) {
      printf("\tthe receive request is MPI_REQUEST_NULL now\n");
    } else {
      printf("\tthe receive request still lingers\n");
    }

  }

  MPI_Finalize();

}
