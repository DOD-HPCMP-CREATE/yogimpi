program nonblock
  implicit none

include 'yogimpif.h'
  
  integer :: pool_size, my_rank, my_name_length, ierr, count
  integer :: status(YogiMPI_STATUS_SIZE), request
  character(128) :: send_buffer, recv_buffer
  character(YogiMPI_MAX_PROCESSOR_NAME) :: my_cpu_name

  call YogiMPI_Init(ierr);
  call YogiMPI_Comm_size(YogiMPI_COMM_WORLD, pool_size, ierr);
  call YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, my_rank, ierr);
  
  if (my_rank == 0) then

    call YogiMPI_Get_processor_name(my_cpu_name, my_name_length, ierr);
    send_buffer = "Hi, this is Fortran from " // my_cpu_name // &
                  ", I hope I do not break."
    call YogiMPI_Isend(send_buffer, 128, YogiMPI_CHARACTER, 1, 77, &
                       YogiMPI_COMM_WORLD, request, ierr)
    print *, "I just sent something. Now I am relaxing to the max."
    call YogiMPI_Wait(request, status, ierr)
    print *, "It looks like the message was sent."

    if (request == YogiMPI_REQUEST_NULL) then 
        print *, "The send request is YogiMPI_REQUEST_NULL now."
    else
        print *, "The send request still lingers."
    endif 

  endif

  if (my_rank == 1) then 

    call YogiMPI_Get_processor_name(my_cpu_name, my_name_length, ierr);
    call YogiMPI_Irecv(recv_buffer, 128, YogiMPI_CHARACTER, 0, 77, &
                       YogiMPI_COMM_WORLD, request, ierr)
    print *, "I am waiting to receive data at ", my_cpu_name, &
            ", so I am relaxing."
    call YogiMPI_Wait(request, status, ierr);
    call YogiMPI_Get_count(status, YogiMPI_CHARACTER, count, ierr);
    print *, "A message just arrived, and the size is ", count
    print *, trim(recv_buffer)

    if (request == YogiMPI_REQUEST_NULL) then 
        print *, "The receive request is YogiMPI_REQUEST_NULL now."
    else
        print *, "The request still lingers."
    endif

  endif

  call YogiMPI_Finalize(ierr);

end program
