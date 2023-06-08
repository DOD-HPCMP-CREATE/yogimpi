program waitsome

use mpi

    integer :: rank, size, i, count, remaining, ierr
    integer :: index(4)
    integer :: sendBuffer(4), receiveBuffer(4)
    integer :: request(4)
    integer :: status(MPI_STATUS_SIZE, 4)

    call MPI_Init(ierr)
    call MPI_Comm_size(MPI_COMM_WORLD, size, ierr)
    if (size /= 4) then
        print *, "Please run with 4 processes."
        call MPI_Finalize(ierr)
        call exit(1)
    end if

    do i=1,size
        sendBuffer(i) = ((i - 1) * 11) + 42
    enddo

    call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)

    if (rank == 0) then
        do i=1,size-1
            call MPI_Isend(sendBuffer(i+1), 1, MPI_INTEGER, i, 123, &
                           MPI_COMM_WORLD, request(i), ierr)
        enddo
        remaining = size-1
        do while (remaining > 0)
            call MPI_Waitsome(size-1, request, count, index, MPI_STATUSES_IGNORE, ierr)
            if (count == MPI_UNDEFINED) then
                call exit(2)
            endif
            if (count > 0) then
                remaining = remaining - count
            endif
        enddo
    else
        call MPI_Recv(receiveBuffer, 1, MPI_INTEGER, 0, 123, MPI_COMM_WORLD, &
                      status(1,1), ierr)
        if (receiveBuffer(1) /= sendBuffer(rank + 1)) then
            call exit(3)
        endif
    endif

    call MPI_Finalize(ierr)

end program waitsome
