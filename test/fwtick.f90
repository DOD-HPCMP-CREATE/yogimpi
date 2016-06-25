program wtick

    use mpi
    integer :: ierr
    real*8 :: tick, tock

    call MPI_Init(ierr)
    tick = MPI_Wtime()
    tock = MPI_Wtime()
    call MPI_Finalize(ierr)

end program
