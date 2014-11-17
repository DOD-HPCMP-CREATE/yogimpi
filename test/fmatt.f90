program testMatt

    implicit none
include 'yogimpif.h'

    integer :: np, me, ierr, flag, msglen, request, status
    integer :: buf(1)
    
    msglen = -1
    buf(1) = -1

    call YogiMPI_Init(ierr)
    call YogiMPI_Comm_size(YogiMPI_COMM_WORLD, np, ierr)
    call YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, me, ierr)

    if(me > 0) then
        print *, "I am the useless processor ", me, " on ", np
    else
        print *, "I am the working processor ", me, " on ", np
    endif

    call YogiMPI_Barrier(YogiMPI_COMM_WORLD, ierr)

    if (me == 0) then
        buf(1) = 69
        call YogiMPI_Isend(buf, 1, YogiMPI_INTEGER, 1, 1, YogiMPI_COMM_WORLD, request, ierr)
        call YogiMPI_Wait(request, status, ierr)
    endif

    call YogiMPI_Barrier(YogiMPI_COMM_WORLD, ierr)

    if (me == 1) then

        call YogiMPI_Irecv(buf, 1, YogiMPI_INTEGER, 0, 1, YogiMPI_COMM_WORLD, request, ierr)
        call YogiMPI_Wait(request, status, ierr)
        call YogiMPI_Get_count(status, YogiMPI_INTEGER, msglen, ierr)
        if(msglen /= 1) then
            print *, "ERROR: The length of the message is not 1"
        else
            print *, "SUCCESS!"
        endif
 
    endif

    call YogiMPI_Finalize(ierr)

end program testMatt
