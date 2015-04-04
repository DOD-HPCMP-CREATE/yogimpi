program waitsome

include 'mpif.h'

    integer :: rank, size, i, count, remaining, ierr
    integer :: index(4)
    integer :: buffer(400)
    integer :: request(4)
    integer :: status(MPI_STATUS_SIZE, 4)
    integer :: MAGIC_NUMBER

    MAGIC_NUMBER = 42

    call MPI_Init(ierr)
    call MPI_Comm_size(MPI_COMM_WORLD, size, ierr)
    if (size /= 4) then
        print *, "Please run with 4 processes."
        call MPI_Finalize(ierr)
        stop 
    end if
    call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)

    if (rank == 0) then
        do i=1,size*100
            buffer(i) = (i-1) + MAGIC_NUMBER 
        end do
        do i=1,size-1
            call MPI_Isend(buffer((i*100)+1), 100, MPI_INTEGER, i, &
                               123, MPI_COMM_WORLD, request(i), ierr)
        end do 
        remaining = size-1
        do while (remaining > 0)
            call MPI_Waitsome(size-1, request, count, index, status, ierr)
            if (count > 0) then
                print *, count, " sends completed"
                remaining = remaining - count
            end if
        end do 
    else
        call MPI_Recv(buffer, 100, MPI_INTEGER, 0, 123, MPI_COMM_WORLD, &
                      status(1,1), ierr)
        print *, rank, ": buffer(1) = ", buffer(1)
    end if

    call MPI_Finalize(ierr)

end program waitsome
