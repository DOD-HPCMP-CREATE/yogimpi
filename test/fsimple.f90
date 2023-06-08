program fsimple
  implicit none
  include 'mpif.h'
  integer :: ierror, rank
  logical :: initialized
  call MPI_Init(ierror)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierror)
  call MPI_Initialized(initialized, ierror)
  if (initialized .neqv. .true.) then
    call exit(1)
  end if
  call MPI_Finalize(ierror)

end program
