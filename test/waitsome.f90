program waitsome

include 'yogimpif.h'

    integer :: rank, size, i, count, remaining, ierr
    integer :: index(4)
    integer :: buffer(400)
    integer :: request(4)
    integer :: status(YogiMPI_STATUS_SIZE, 4)
    integer :: MAGIC_NUMBER

    MAGIC_NUMBER = 42

    call YogiMPI_Init(ierr)
    call YogiMPI_Comm_size(YogiMPI_COMM_WORLD, size, ierr)
    if (size /= 4) then
        print *, "Please run with 4 processes."
        call YogiMPI_Finalize(ierr)
        stop 
    end if
    call YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, rank, ierr)

    if (rank == 0) then
        do i=1,size*100
            buffer(i) = (i-1) + MAGIC_NUMBER 
        end do
        do i=1,size-1
            call YogiMPI_Isend(buffer((i*100)+1), 100, YogiMPI_INTEGER, i, &
                               123, YogiMPI_COMM_WORLD, request(i), ierr)
        end do 
        remaining = size-1
        do while (remaining > 0)
            call YogiMPI_Waitsome(size-1, request, count, index, status, ierr)
            if (count > 0) then
                print *, count, " sends completed"
                remaining = remaining - count
            end if
        end do 
    else
        call YogiMPI_Recv(buffer, 100, YogiMPI_INTEGER, 0, 123, YogiMPI_COMM_WORLD, &
                      status(1,1), ierr)
        print *, rank, ": buffer(1) = ", buffer(1)
    end if

    call YogiMPI_Finalize(ierr)

end program waitsome
