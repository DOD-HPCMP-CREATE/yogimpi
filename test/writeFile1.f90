program writeFile1

include 'yogimpif.h'

    integer :: i, rank, size, offset, nints, N, ierr
    integer :: fhw, buf(16)
    integer :: status(YogiMPI_STATUS_SIZE)

    N = 16

    call YogiMPI_INIT(ierr)
    call YogiMPI_COMM_RANK(YogiMPI_COMM_WORLD, rank, ierr)
    call YogiMPI_COMM_SIZE(YogiMPI_COMM_WORLD, size, ierr)

    do i=1,N
        buf(i) = i - 1 
    enddo

    offset = rank*(N/size)*4
    call YogiMPI_FILE_OPEN(YogiMPI_COMM_WORLD, "datafile", &
                           OR(YogiMPI_MODE_CREATE, YogiMPI_MODE_WRONLY), &
                           YogiMPI_INFO_NULL, fhw, ierr)
    print *, "here"
    print *, "Rank: ", rank, " Offset: ", offset
    call YogiMPI_FILE_WRITE_AT(fhw, offset, buf, (N/size), YogiMPI_INTEGER, status, ierr)
    call YogiMPI_FILE_CLOSE(fhw, ierr);
    call YogiMPI_FINALIZE(ierr);
end program
