program hello

    implicit none

include "yogimpif.h"
    integer myid, ierr, numprocs
    integer tag, source, destination, count
    integer buffer
    integer status(YogiMPI_STATUS_SIZE)
    call YogiMPI_INIT(ierr)
    call YogiMPI_COMM_RANK(YogiMPI_COMM_WORLD, myid, ierr)
    call YogiMPI_COMM_SIZE(YogiMPI_COMM_WORLD, numprocs, ierr)
    tag=1234
    source=0
    destination=1
    count=1

    if(myid .eq. source) then
        buffer=5678
        Call YogiMPI_Send(buffer, count, YogiMPI_INTEGER,destination,&
        tag, YogiMPI_COMM_WORLD, ierr)
        write(*,*)"processor ",myid," sent ",buffer
    endif

    if(myid .eq. destination) then
        Call YogiMPI_Recv(buffer, count, YogiMPI_INTEGER,source,&
        tag, YogiMPI_COMM_WORLD, status,ierr)
        write(*,*)"processor ",myid," got ",buffer
    endif

    call YogiMPI_FINALIZE(ierr)
end program
