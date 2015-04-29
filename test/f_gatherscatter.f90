! This program shows how to use MPI_Scatter and MPI_Gather
! Each processor gets different data from the root processor
! by way of mpi_scatter.  The data is summed and then sent back
! to the root processor using MPI_Gather.  The root processor
! then prints the global sum. 
program test1
    implicit none
    include 'mpif.h'
    integer :: numnodes, myid, mpi_err, expected
    integer, parameter :: my_root=0
    integer, allocatable :: myray(:),send_ray(:),back_ray(:)
    integer count
    integer :: size, mysize, i, k, j, total
    
    call MPI_INIT(mpi_err)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, numnodes, mpi_err)
    call MPI_COMM_RANK(MPI_COMM_WORLD, myid, mpi_err)
    
    ! each processor will get count elements from the root
    count=4
    allocate(myray(count))
    ! create the data to be sent on the root
    if(myid == my_root)then
        size=count*numnodes
        allocate(send_ray(0:size-1))
        allocate(back_ray(0:numnodes-1))
        do i=0,size-1
            send_ray(i)=i
        enddo
    endif
    ! send different data to each processor 
    call MPI_SCATTER(send_ray, count, MPI_INTEGER, myray, count, &
                      MPI_INTEGER, my_root, MPI_COMM_WORLD, mpi_err)
                
    ! each processor does a local sum
    total=sum(myray)

    ! send the local sums back to the root
    call MPI_GATHER(total, 1 ,MPI_INTEGER, back_ray, 1, MPI_INTEGER, &
                    my_root, MPI_COMM_WORLD, mpi_err)

    if(myid == my_root) then
        expected = sum(send_ray)
        if (expected /= sum(back_ray)) then
            call exit(2)
        endif
    endif

    call MPI_FINALIZE(mpi_err)

end program
