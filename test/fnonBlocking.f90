program nonblock
  implicit none

include 'mpif.h'
  
  integer :: pool_size, my_rank, my_name_length, ierr, count
  integer :: status(MPI_STATUS_SIZE), request
  character(128) :: send_buffer, recv_buffer
  character(MPI_MAX_PROCESSOR_NAME) :: my_cpu_name

  call MPI_Init(ierr);
  call MPI_Comm_size(MPI_COMM_WORLD, pool_size, ierr);
  call MPI_Comm_rank(MPI_COMM_WORLD, my_rank, ierr);
  
  if (my_rank == 0) then

    call MPI_Get_processor_name(my_cpu_name, my_name_length, ierr);
    send_buffer = "Hi, this is Fortran from " // my_cpu_name // &
                  ", I hope I do not break."
    call MPI_Isend(send_buffer, 128, MPI_CHARACTER, 1, 77, &
                       MPI_COMM_WORLD, request, ierr)
    print *, "I just sent something. Now I am relaxing to the max."
    call MPI_Wait(request, status, ierr)
    print *, "It looks like the message was sent."

    if (request == MPI_REQUEST_NULL) then 
        print *, "The send request is MPI_REQUEST_NULL now."
    else
        print *, "The send request still lingers."
    endif 

  endif

  if (my_rank == 1) then 

    call MPI_Get_processor_name(my_cpu_name, my_name_length, ierr);
    call MPI_Irecv(recv_buffer, 128, MPI_CHARACTER, 0, 77, &
                       MPI_COMM_WORLD, request, ierr)
    print *, "I am waiting to receive data at ", my_cpu_name, &
            ", so I am relaxing."
    call MPI_Wait(request, status, ierr);
    call MPI_Get_count(status, MPI_CHARACTER, count, ierr);
    print *, "A message just arrived, and the size is ", count
    print *, trim(recv_buffer)

    if (request == MPI_REQUEST_NULL) then 
        print *, "The receive request is MPI_REQUEST_NULL now."
    else
        print *, "The request still lingers."
    endif

  endif

  call MPI_Finalize(ierr);

end program
