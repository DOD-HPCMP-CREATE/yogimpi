program collective

include 'yogimpif.h'

    integer :: rank, new_rank, sendbuf, recvbuf, NPROCS
    integer :: ranks1(4), ranks2(4)
    integer :: orig_group, new_group, new_comm, ierr

    NPROCS = 8
    ranks1(1:4) = (/0,1,2,3/)
    ranks2(1:4) = (/4,5,6,7/)

    call MPI_INIT(ierr) 
    call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
    sendbuf = rank

    ! Extract the original group handle

    call YogiMPI_Comm_group(YogiMPI_COMM_WORLD, orig_group, ierr)
      
    ! Divide tasks into two distinct groups based upon rank

    if (rank < NPROCS/2) then 
        call MPI_GROUP_INCL(orig_group, NPROCS/2, ranks1, new_group, ierr)
    else 
        call MPI_GROUP_INCL(orig_group, NPROCS/2, ranks2, new_group, ierr)
    end if

    ! Create new communicator and then perform collective communications

    call MPI_COMM_CREATE(MPI_COMM_WORLD, new_group, new_comm, ierr)
    call MPI_ALLREDUCE(sendbuf, recvbuf, 1, MPI_INTEGER, MPI_SUM, &
                           new_comm, ierr) 
    call MPI_GROUP_RANK(new_group, new_rank, ierr) 

    print *, "rank= ", rank, " newrank= ", new_rank, " recvbuf= ", recvbuf

    call MPI_FINALIZE(ierr)

end program collective
