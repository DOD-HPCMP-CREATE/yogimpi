!A simple MPI example program using non-blocking send and receive
!and MPI_Waitall to wait for completion of all message transfers.

!The program consists of one processes (process 0) which sends a message
!containing its identifier to N-1 other processes. These receive the
!message and send it back. All processes use non-blocking send and receive
!operations (MPI_Isend and MPI_Irecv, and MPI_Waitall to wait until the
!messages have arrived).

!Run the program with at least 2 processes.

program nonblock_waitall

    implicit none
include "mpif.h"

    integer, parameter :: MAXPROC=8
    integer :: i, x, np, me, y(MAXPROC), ierr, count
    integer :: tag
    integer :: status(MPI_STATUS_SIZE, MAXPROC)
    integer :: send_req(MAXPROC)
    integer :: recv_req(MAXPROC)
    tag = 42;

    call MPI_Init(ierr)
    call MPI_Comm_size(MPI_COMM_WORLD, np, ierr)
    call MPI_Comm_rank(MPI_COMM_WORLD, me, ierr)

    ! First check that we have at least 2 and at most MAXPROC processes
    if (np < 2 .or. np > MAXPROC) then
        if (me == 0) then 
            print *, "Aborting.  Use between 2 and 8 processes."
        endif
        call MPI_Finalize(ierr)
        stop 
    endif
  
    x = me

    if (me == 0) then 
        print *, "Process 0 sending to all other processes:"
        do i = 1,np-1
            print *, "0: Sending to ", i
            call MPI_Isend(x, 1, MPI_INTEGER, i, tag, &
                               MPI_COMM_WORLD, send_req(i + 1), ierr)
        enddo 

        ! Note that we use requests and statuses starting from index 2 
        ! since process zero does not send to it self
        call MPI_Waitall(np-1, send_req(2), status(:,2), ierr)

        print *, "Process 0 receiving from all other processes"
        do i = 2, np
            call MPI_Irecv(y(i), 1, MPI_INTEGER, &
                               MPI_ANY_SOURCE, tag, &
                               MPI_COMM_WORLD, recv_req(i), ierr)
        end do 
        call MPI_Waitall(np - 1, recv_req(2), status(:,2), ierr)

        do i = 2, np
            call MPI_Get_count(status(:,i), &
                                   MPI_INTEGER, count, ierr)
            print *, "Process 0 received a message from process ", y(i), &
                     ", size ", count 
        end do

        print *, "Process 0 complete."
    
    else

        call MPI_Irecv(y, 1, MPI_INTEGER, 0, tag, MPI_COMM_WORLD, & 
                       recv_req(1), ierr)
        call MPI_Wait(recv_req(1), status(1,1), ierr)
        call MPI_Isend(x, 1, MPI_INTEGER, 0, tag, MPI_COMM_WORLD, &
                       send_req(1), ierr)
        call MPI_Wait(send_req(1), status(1,1), ierr)

    endif 

    call MPI_Finalize(ierr)

end program
