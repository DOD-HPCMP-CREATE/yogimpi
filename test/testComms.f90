program testComms

include 'yogimpif.h'

    integer :: dup_comm_world, world_comm
    integer :: world_group
    integer :: world_rank, world_size, rank, size, ierr

    call YogiMPI_Init(ierr)
    call YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, world_rank, ierr)
    call YogiMPI_Comm_size(YogiMPI_COMM_WORLD, world_size, ierr)
    call YogiMPI_Comm_dup(YogiMPI_COMM_WORLD, dup_comm_world, ierr)
    ! Exercise Comm_create by creating an equivalent to dup_comm_world
    ! (sans attributes)
    call YogiMPI_Comm_group(dup_comm_world, world_group, ierr)
    call YogiMPI_Comm_create(dup_comm_world, world_group, world_comm, ierr)
    call YogiMPI_Comm_rank(world_comm, rank, ierr)
    if (rank /= world_rank) then
        print *, "incorrect rank in world comm: ", rank
        call YogiMPI_Abort(YogiMPI_COMM_WORLD, 3001, ierr)
    end if 
    call YogiMPI_Finalize(ierr)

end program testComms
