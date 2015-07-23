!
!YogiMPI Library - MPI ABI Translator
!Copyright (C) 2006, 2007 Toon Knapen Free Field Technologies S.A.
!Additions made by Stephen Adamec, University of Alabama at Birmingham

!This library is free software; you can redistribute it and/or
!modify it under the terms of the GNU Lesser General Public
!License as published by the Free Software Foundation; either
!version 2.1 of the License, or (at your option) any later version.

!This library is distributed in the hope that it will be useful,
!but WITHOUT ANY WARRANTY; without even the implied warranty of
!MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
!Lesser General Public License for more details.

!You should have received a copy of the GNU Lesser General Public
!License along with this library; if not, write to the Free Software
!Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

! File to provide extra Fortran compatibility for YogiMPI */

subroutine YogiMPI_Test(request, flag, status, ierror)

  implicit none
  integer, parameter :: YogiMPI_STATUS_SIZE = 15
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
  integer, parameter :: YogiMPI_STATUS_SIZE = 15
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
