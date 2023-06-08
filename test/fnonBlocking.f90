program nonblock
  implicit none

include 'mpif.h'

  integer :: my_rank, ierr, count, send_length
  integer :: status(MPI_STATUS_SIZE), request
  character(128) :: send_buffer, recv_buffer

  call MPI_Init(ierr);
  call MPI_Comm_rank(MPI_COMM_WORLD, my_rank, ierr);

  send_buffer = "This is a test message to be sent."
  send_length = len(trim(send_buffer))

  if (my_rank == 0) then

    call MPI_Isend(send_buffer, 128, MPI_CHARACTER, 1, 77, MPI_COMM_WORLD, &
                   request, ierr)

    call MPI_Wait(request, status, ierr)
    if (request /= MPI_REQUEST_NULL) then
        call exit(3)
    endif

  endif

  if (my_rank == 1) then
    call MPI_Irecv(recv_buffer, 128, MPI_CHARACTER, 0, 77, MPI_COMM_WORLD, &
                   request, ierr)
    call MPI_Wait(request, status, ierr);

    call MPI_Get_count(status, MPI_CHARACTER, count, ierr);

    if (count /= 128) then
        call exit(5)
    endif

    if(len(trim(recv_buffer)) /= len(trim(send_buffer))) then
        call exit(1)
    endif

    if(trim(recv_buffer) /= trim(send_buffer)) then
        call exit(2)
    endif

    if (request /= MPI_REQUEST_NULL) then
        call exit(4)
    endif

  endif

  call MPI_Finalize(ierr);

end program
