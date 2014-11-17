!A simple MPI example program using non-blocking send and receive
!and YogiMPI_Waitall to wait for completion of all message transfers.

!The program consists of one processes (process 0) which sends a message
!containing its identifier to N-1 other processes. These receive the
!message and send it back. All processes use non-blocking send and receive
!operations (YogiMPI_Isend and YogiMPI_Irecv, and YogiMPI_Waitall to wait until the
!messages have arrived).

!Run the program with at least 2 processes.

program nonblock_waitall

    implicit none
include "yogimpif.h"

    integer, parameter :: MAXPROC=8
    integer :: i, x, np, me, y(MAXPROC), ierr
    integer :: tag
    integer :: status(YogiMPI_STATUS_SIZE, MAXPROC) 
    integer :: send_req(MAXPROC)
    integer :: recv_req(MAXPROC)
    tag = 42;

    call YogiMPI_Init(ierr)
    call YogiMPI_Comm_size(YogiMPI_COMM_WORLD, np, ierr)
    call YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, me, ierr)

    ! First check that we have at least 2 and at most MAXPROC processes
    if (np < 2 .or. np > MAXPROC) then
        if (me == 0) then 
            print *, "Aborting.  Use between 2 and 8 processes."
        endif
        call YogiMPI_Finalize(ierr)
        stop 
    endif
  
    x = me

    if (me == 0) then 
        print *, "Process 0 sending to all other processes:"
        do i = 1, np - 1
            print *, "0: Sending to ", i
            call YogiMPI_Isend(x, 1, YogiMPI_INTEGER, i, tag, YogiMPI_COMM_WORLD, &
                           send_req(i + 1), ierr)
        enddo 

        ! Note that we use requests and statuses starting from index 2 
        ! since process zero does not send to it self
        call YogiMPI_Waitall(np-1, send_req(2), status(2,:), ierr)

        print *, "Process 0 receiving from all other processes"
        do i = 1, np - 1
            call YogiMPI_Irecv(y(i + 1), 1, YogiMPI_INTEGER, &
                               YogiMPI_ANY_SOURCE, tag, &
                               YogiMPI_COMM_WORLD, recv_req(i + 1), ierr)
        end do 
        call YogiMPI_Waitall(np - 1, recv_req(2), status(2,:), ierr)

        do i = 1, np - 1
            print *, "Process 0 received a message from process ", y(i)
        end do

        print *, "Process 0 complete."
    
    else

        call YogiMPI_Irecv(y, 1, YogiMPI_INTEGER, 0, tag, YogiMPI_COMM_WORLD, & 
                       recv_req(1), ierr)
        call YogiMPI_Wait(recv_req(1), status(1,1), ierr)
        call YogiMPI_Isend(x, 1, YogiMPI_INTEGER, 0, tag, YogiMPI_COMM_WORLD, &
                       send_req(1), ierr)
        call YogiMPI_Wait(send_req(1), status(1,1), ierr)

    endif 

    call YogiMPI_Finalize(ierr)

end program
