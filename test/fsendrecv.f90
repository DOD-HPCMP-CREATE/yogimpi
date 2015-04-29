program fsendrecv 

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
        call MPI_Send(buffer, count, MPI_INTEGER, destination, tag, &
                      MPI_COMM_WORLD, ierr)
    endif

    if(myid .eq. destination) then
        call MPI_Recv(buffer, count, MPI_INTEGER, source, tag, MPI_COMM_WORLD,&
                      status, ierr)
        if(buffer /= 5678) then
            call exit(1)
        endif
    endif

    call MPI_FINALIZE(ierr)

end program
