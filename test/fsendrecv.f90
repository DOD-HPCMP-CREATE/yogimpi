program hello

    implicit none

include "mpif.h"
    integer myid, ierr, numprocs
    integer tag, source, destination, count
    integer buffer
    integer status(MPI_STATUS_SIZE)
    call MPI_INIT(ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierr)
    tag=1234
    source=0
    destination=1
    count=1

    if(myid .eq. source) then
        buffer=5678
        Call MPI_Send(buffer, count, MPI_INTEGER,destination,&
        tag, MPI_COMM_WORLD, ierr)
        write(*,*)"processor ",myid," sent ",buffer
    endif

    if(myid .eq. destination) then
        Call MPI_Recv(buffer, count, MPI_INTEGER,source,&
        tag, MPI_COMM_WORLD, status,ierr)
        write(*,*)"processor ",myid," got ",buffer
    endif

    call MPI_FINALIZE(ierr)
end program
