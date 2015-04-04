program testComms

include 'mpif.h'

    integer :: dup_comm_world, world_comm
    integer :: world_group
    integer :: world_rank, world_size, rank, size, ierr

    call MPI_INIT(ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, world_rank, ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, world_size, ierr)
    call MPI_COMM_DUP(MPI_COMM_WORLD, dup_comm_world, ierr)
    ! Exercise Comm_create by creating an equivalent to dup_comm_world
    ! (sans attributes)
    call MPI_COMM_GROUP(dup_comm_world, world_group, ierr)
    call MPI_COMM_CREATE(dup_comm_world, world_group, world_comm, ierr)
    call MPI_COMM_RANK(world_comm, rank, ierr)
    if (rank /= world_rank) then
        print *, "incorrect rank in world comm: ", rank
        call MPI_ABORT(MPI_COMM_WORLD, 3001, ierr)
    end if 
    call MPI_FINALIZE(ierr)

end program testComms
