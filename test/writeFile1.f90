program writeFile1

include 'mpif.h'

    integer :: i, rank, size, nints, N, ierr
    integer(kind=MPI_OFFSET_KIND) :: offset
    integer :: fhw, buf(16)
    integer :: status(MPI_STATUS_SIZE)

    N = 16

    call MPI_INIT(ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, size, ierr)

    do i=1,N
        buf(i) = i - 1 
    enddo

    offset = rank*(N/size)*4
    call MPI_FILE_OPEN(MPI_COMM_WORLD, "fWriteFile.result", &
                           OR(MPI_MODE_CREATE, MPI_MODE_WRONLY), &
                           MPI_INFO_NULL, fhw, ierr)
    call MPI_FILE_WRITE_AT(fhw, offset, buf, (N/size), MPI_INTEGER, status, ierr)
    call MPI_FILE_CLOSE(fhw, ierr);
    call MPI_FINALIZE(ierr);
end program
