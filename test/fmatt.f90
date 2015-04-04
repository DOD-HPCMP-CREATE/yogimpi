program testMatt

    implicit none
include 'mpif.h'

    integer :: np, me, ierr, flag, msglen, request, status
    integer :: buf(1)
    
    msglen = -1
    buf(1) = -1

    call MPI_INIT(ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, np, ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, me, ierr)

    if(me > 0) then
        print *, "I am the useless processor ", me, " on ", np
    else
        print *, "I am the working processor ", me, " on ", np
    endif

    call MPI_BARRIER(MPI_COMM_WORLD, ierr)

    if (me == 0) then
        buf(1) = 69
        call MPI_ISEND(buf, 1, MPI_INTEGER, 1, 1, MPI_COMM_WORLD, request, ierr)
        call MPI_WAIT(request, status, ierr)
    endif

    call MPI_BARRIER(MPI_COMM_WORLD, ierr)

    if (me == 1) then

        call MPI_IRECV(buf, 1, MPI_INTEGER, 0, 1, MPI_COMM_WORLD, request, ierr)
        call MPI_WAIT(request, status, ierr)
        call MPI_GET_COUNT(status, MPI_INTEGER, msglen, ierr)
        if(msglen /= 1) then
            print *, "ERROR: The length of the message is not 1"
        else
            print *, "SUCCESS!"
        endif
 
    endif

    call MPI_FINALIZE(ierr)

end program testMatt
