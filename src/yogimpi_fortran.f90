! File to provide extra Fortran compatibility for YogiMPI

subroutine YogiMPI_Test(request, flag, status, ierror)

  implicit none
  include 'yogimpif.h'
  logical, intent(out) :: flag 
  integer, intent(in) :: request
  integer, intent(out) :: status(YogiMPI_STATUS_SIZE), ierror 

  integer :: cflag

  call f90bridge_YogiMPI_Test(request, cflag, status, ierror)

  if (cflag /= 0) then
    flag = .true.
  else
    flag = .false.
  end if

end subroutine

subroutine YogiMPI_IProbe(source, tag, comm, flag, status, ierror)
  
  implicit none
  include 'yogimpif.h'
  logical, intent(out) :: flag
  integer, intent(in) :: source, tag, comm
  integer, intent(out) :: status(YogiMPI_STATUS_SIZE), ierror 

  integer :: cflag

  call f90bridge_YogiMPI_IProbe(source, tag, comm, cflag, status, ierror)

  if (cflag /= 0) then
    flag = .true.
  else
    flag = .false.
  end if
  
end subroutine

subroutine YogiMPI_Attr_get(comm, keyval, attribute_val, flag, ierror)

  implicit none
  integer, intent(in) :: comm, keyval
  integer, intent(out) :: attribute_val, ierror
  logical, intent(out) :: flag 

  ! Temporary integer to hold C flag
  integer :: cflag

  call f90bridge_YogiMPI_Attr_get(comm, keyval, attribute_val, cflag, ierror)

  if (cflag /= 0) then
    flag = .true.
  else
    flag = .false.
  end if

end subroutine

subroutine YogiMPI_Initialized(flag, ierror)

  implicit none
  logical, intent(out) :: flag
  integer, intent(out) :: ierror
  
  ! Temporary integer to hold C flag
  integer :: cflag
  
  call f90bridge_YogiMPI_Initialized(cflag, ierror)

  if (cflag /= 0) then
    flag = .true.
  else
    flag = .false.
  end if  

end subroutine

subroutine YogiMPI_Finalized(flag, ierror)

  implicit none
  logical, intent(out) :: flag 
  integer, intent(out) :: ierror 

  ! Temporary integer to hold C flag
  integer :: cflag

  call f90bridge_YogiMPI_Finalized(cflag, ierror)

  if (cflag /= 0) then
    flag = .true.
  else
    flag = .false.
  end if
          
end subroutine
