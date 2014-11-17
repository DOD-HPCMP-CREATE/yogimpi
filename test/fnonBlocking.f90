program nonblock

include 'yogimpif.h"
  
  integer :: pool_size, my_rank, ierr

  YogiMPI_Init();
  YogiMPI_Comm_size(YogiMPI_COMM_WORLD, pool_size, ierr);
  YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, my_rank, ierr);
  
  if (my_rank == 0) then

    char send_buffer[BUFSIZ], my_cpu_name[BUFSIZ];
    int my_name_length;
    YogiMPI_Request request;
    YogiMPI_Status status;

    YogiMPI_Get_processor_name(my_cpu_name, &my_name_length);
    sprintf (send_buffer, "Dear Task 1,\n\
Please do not send any more messages.\n\
Please send money instead.\n\
\tYours faithfully,\n\
\tTask 0\n\
\tRunning on %s\n", my_cpu_name);
    YogiMPI_Isend (send_buffer, strlen(send_buffer) + 1, YogiMPI_CHAR,
               1, 77, YogiMPI_COMM_WORLD, &request);
    printf("hello there user, I've just started this send\n\
and I'm having a good time relaxing.\n");
    YogiMPI_Wait (&request, &status);
    printf("hello there user, it looks like the message has been sent.\n");

    if (request == YogiMPI_REQUEST_NULL) {
      printf("\tthe send request is YogiMPI_REQUEST_NULL now\n");
    } else {
      printf("\tthe send request still lingers\n");
    }

  } 
  else if (my_rank == 1) {

    char recv_buffer[BUFSIZ], my_cpu_name[BUFSIZ];
    int my_name_length, count;
    YogiMPI_Request request;
    YogiMPI_Status status;
    
    YogiMPI_Get_processor_name(my_cpu_name, &my_name_length);
    YogiMPI_Irecv (recv_buffer, BUFSIZ, YogiMPI_CHAR, 0, 77, YogiMPI_COMM_WORLD,
               &request);
    printf("hello there user, I've just started this receive\n\
on %s, and I'm having a good time relaxing.\n", my_cpu_name);
    YogiMPI_Wait(&request, &status);
    YogiMPI_Get_count(&status, YogiMPI_CHAR, &count);
    printf("hello there user, it looks like %d characters \
have just arrived:\n", count );
    printf("%s", recv_buffer);

    if (request == YogiMPI_REQUEST_NULL) {
      printf("\tthe receive request is YogiMPI_REQUEST_NULL now\n");
    } else {
      printf("\tthe receive request still lingers\n");
    }

  }

  YogiMPI_Finalize();

}
