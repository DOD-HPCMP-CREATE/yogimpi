! This program shows how to use fmpi_Scatter and YogiMPI_Gather
! Each processor gets different data from the root processor
! by way of mpi_scatter.  The data is summed and then sent back
! to the root processor using YogiMPI_Gather.  The root processor
! then prints the global sum. 
program test1
    implicit none
    include 'yogimpif.h'
    integer :: numnodes, myid, mpi_err
    integer, parameter :: my_root=0
    integer, allocatable :: myray(:),send_ray(:),back_ray(:)
    integer count
    integer :: size, mysize, i, k, j, total
    
    call YogiMPI_INIT(mpi_err)
    call YogiMPI_COMM_SIZE(YogiMPI_COMM_WORLD, numnodes, mpi_err)
    call YogiMPI_Comm_rank(YogiMPI_COMM_WORLD, myid, mpi_err)
    
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
    call YogiMPI_Scatter(send_ray, count, YogiMPI_INTEGER, myray, count, &
                      YogiMPI_INTEGER, my_root, YogiMPI_COMM_WORLD, mpi_err)
	                
    ! each processor does a local sum
    total=sum(myray)
    write(*,*)"myid= ",myid," total= ",total
    ! send the local sums back to the root
    call YogiMPI_Gather(total, 1 ,YogiMPI_INTEGER, back_ray, 1, YogiMPI_INTEGER, &
                    my_root, YogiMPI_COMM_WORLD, mpi_err)
    ! the root prints the global sum
    if(myid == my_root)then
        write(*,*)"results from all processors= ",sum(back_ray)
    endif

    call YogiMPI_finalize(mpi_err)

end program
