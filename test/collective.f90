program collective

include 'yogimpif.h'

    integer :: rank, new_rank, sendbuf, recvbuf, NPROCS
    integer :: ranks1(4), ranks2(4)
    integer :: orig_group, new_group, new_comm, ierr

    NPROCS = 8
    ranks1(1:4) = (/0,1,2,3/)
    ranks2(1:4) = (/4,5,6,7/)

    call YogiMPI_Init(ierr) 
    call YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, rank, ierr)
    sendbuf = rank

    ! Extract the original group handle

    call YogiMPI_Comm_group(YogiMPI_COMM_WORLD, orig_group, ierr)
      
    ! Divide tasks into two distinct groups based upon rank

    if (rank < NPROCS/2) then 
        call YogiMPI_Group_incl(orig_group, NPROCS/2, ranks1, new_group, ierr)
    else 
        call YogiMPI_Group_incl(orig_group, NPROCS/2, ranks2, new_group, ierr)
    end if

    ! Create new communicator and then perform collective communications

    call YogiMPI_Comm_create(YogiMPI_COMM_WORLD, new_group, new_comm, ierr)
    call YogiMPI_Allreduce(sendbuf, recvbuf, 1, YogiMPI_INTEGER, YogiMPI_SUM, &
                           new_comm, ierr) 
    call YogiMPI_Group_rank(new_group, new_rank, ierr) 

    print *, "rank= ", rank, " newrank= ", new_rank, " recvbuf= ", recvbuf

    call YogiMPI_Finalize(ierr)

end program collective
