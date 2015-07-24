! YogiMPI Library - MPI ABI Translator
! Copyright (C) 2006, 2007 Toon Knapen Free Field Technologies S.A.
! Additions made by Stephen Adamec, University of Alabama at Birmingham

! This library is free software; you can redistribute it and/or
! modify it under the terms of the GNU Lesser General Public
! License as published by the Free Software Foundation; either
! version 2.1 of the License, or (at your option) any later version.

! This library is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
! Lesser General Public License for more details.

! You should have received a copy of the GNU Lesser General Public
! License along with this library; if not, write to the Free Software
! Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

! Defined MPI constants for Fortran

! return codes (both C and Fortran)
      integer, parameter :: YogiMPI_SUCCESS = 0
      integer, parameter :: YogiMPI_ERR_BUFFER = 1
      integer, parameter :: YogiMPI_ERR_COUNT = 2
      integer, parameter :: YogiMPI_ERR_TYPE = 3
      integer, parameter :: YogiMPI_ERR_TAG = 4 
      integer, parameter :: YogiMPI_ERR_COMM = 5 
      integer, parameter :: YogiMPI_ERR_RANK = 6 
      integer, parameter :: YogiMPI_ERR_REQUEST = 7 
      integer, parameter :: YogiMPI_ERR_ROOT = 8
      integer, parameter :: YogiMPI_ERR_GROUP = 9 
      integer, parameter :: YogiMPI_ERR_OP = 10 
      integer, parameter :: YogiMPI_ERR_TOPOLOGY = 11 
      integer, parameter :: YogiMPI_ERR_DIMS = 12 
      integer, parameter :: YogiMPI_ERR_ARG = 13 
      integer, parameter :: YogiMPI_ERR_UNKNOWN = 14 
      integer, parameter :: YogiMPI_ERR_TRUNCATE = 15 
      integer, parameter :: YogiMPI_ERR_OTHER = 16 
      integer, parameter :: YogiMPI_ERR_INTERN = 17
      integer, parameter :: YogiMPI_ERR_PENDING = 18 
      integer, parameter :: YogiMPI_ERR_IN_STATUS = 19 
      integer, parameter :: YogiMPI_ERR_LASTCODE = 20 

! assorted constants (both C and Fortran)
      integer, parameter :: YogiMPI_BOTTOM = -1
      integer, parameter :: YogiMPI_PROC_NULL = -2
      integer, parameter :: YogiMPI_ANY_SOURCE = -3
      integer, parameter :: YogiMPI_ANY_TAG = -4
      integer, parameter :: YogiMPI_UNDEFINED = -5
      integer, parameter :: YogiMPI_BSEND_OVERHEAD = -6
      integer, parameter :: YogiMPI_KEYVAL_INVALID = -7


! status size and reserved index values (Fortran)
! MPI_STATUS_SIZE is 15.  This is so the first three integers may
! be used to provide MPI_SOURCE, MPI_TAG, and MPI_ERROR to users.
! The remaining 12 integer elements house the real MPI_Status object.
      integer, parameter :: YogiMPI_STATUS_SIZE = 15
      integer, parameter :: YogiMPI_SOURCE = 1 
      integer, parameter :: YogiMPI_TAG = 2 
      integer, parameter :: YogiMPI_ERROR = 3 
      integer :: YogiMPI_STATUS_IGNORE(YogiMPI_STATUS_SIZE)
      integer :: YogiMPI_STATUSES_IGNORE(YogiMPI_STATUS_SIZE,1)
! Declare common blocks to hold these special variables.
      COMMON /ympiscalar/ YogiMPI_STATUS_IGNORE
      COMMON /ympiarray/ YogiMPI_STATUSES_IGNORE

! Maximum sizes for strings
! This has to be one less than the value in C to account for null-terminated
! strings.
      integer, parameter :: YogiMPI_MAX_PROCESSOR_NAME = 31

! elementary datatypes (Fortran)
      integer, parameter :: YogiMPI_CHARACTER = 1
      integer, parameter :: YogiMPI_INTEGER = 3
      integer, parameter :: YogiMPI_REAL = 9
      integer, parameter :: YogiMPI_DOUBLE_PRECISION = 10
      integer, parameter :: YogiMPI_BYTE = 12
      integer, parameter :: YogiMPI_PACKED = 13
      integer, parameter :: YogiMPI_COMPLEX = 23
      integer, parameter :: YogiMPI_DOUBLE_COMPLEX = 24
      integer, parameter :: YogiMPI_LOGICAL = 25

! datatypes for reduction functions (Fortran)
      integer, parameter :: YogiMPI_2REAL = 26 
      integer, parameter :: YogiMPI_2DOUBLE_PRECISION = 27 
      integer, parameter :: YogiMPI_2INTEGER = 28

! optional datatypes (Fortran)
      integer, parameter :: YogiMPI_INTEGER1 = 29
      integer, parameter :: YogiMPI_INTEGER2 = 30 
      integer, parameter :: YogiMPI_INTEGER4 = 31 
      integer, parameter :: YogiMPI_INTEGER8 = 32
      integer, parameter :: YogiMPI_REAL4 = 33
      integer, parameter :: YogiMPI_REAL8 = 34 

! reserved communicators (C and Fortran)
      integer, parameter :: YogiMPI_COMM_WORLD = 1
      integer, parameter :: YogiMPI_COMM_SELF = 2

! results of communicator and group comparisons
      integer, parameter :: YogiMPI_IDENT = 0
      integer, parameter :: YogiMPI_CONGRUENT = 1
      integer, parameter :: YogiMPI_SIMILAR = 2
      integer, parameter :: YogiMPI_UNEQUAL = 3

! environmental inquiry keys (C and Fortran)
      integer, parameter :: YogiMPI_TAG_UB = -8
      integer, parameter :: YogiMPI_IO = -9
      integer, parameter :: YogiMPI_HOST = -10
      integer, parameter :: YogiMPI_WTIME_IS_GLOBAL = -11

! collective operations (C and Fortran)
      integer, parameter :: YogiMPI_MAX = 1
      integer, parameter :: YogiMPI_MIN = 2
      integer, parameter :: YogiMPI_SUM = 3
      integer, parameter :: YogiMPI_PROD = 4
      integer, parameter :: YogiMPI_MAXLOC = 5
      integer, parameter :: YogiMPI_MINLOC = 6
      integer, parameter :: YogiMPI_BAND = 7
      integer, parameter :: YogiMPI_BOR = 8
      integer, parameter :: YogiMPI_BXOR = 9
      integer, parameter :: YogiMPI_LAND = 10
      integer, parameter :: YogiMPI_LOR = 11
      integer, parameter :: YogiMPI_LXOR = 12

! Null handles

      integer, parameter :: YogiMPI_GROUP_NULL = 0
      integer, parameter :: YogiMPI_COMM_NULL = 0
      integer, parameter :: YogiMPI_REQUEST_NULL = 0
      integer, parameter :: YogiMPI_DATATYPE_NULL = 0
      integer, parameter :: YogiMPI_INFO_NULL = 0
      integer, parameter :: YogiMPI_FILE_NULL = 0

! Empty group
      integer, parameter :: YogiMPI_GROUP_EMPTY = 3

! MPI I/O permission modes, powers of 2 
      integer, parameter :: YogiMPI_MODE_RDONLY = 2
      integer, parameter :: YogiMPI_MODE_RDWR = 8
      integer, parameter :: YogiMPI_MODE_WRONLY = 4
      integer, parameter :: YogiMPI_MODE_CREATE = 1
      integer, parameter :: YogiMPI_MODE_EXCL = 64
      integer, parameter :: YogiMPI_MODE_DELETE_ON_CLOSE = 16
      integer, parameter :: YogiMPI_MODE_UNIQUE_OPEN = 32
      integer, parameter :: YogiMPI_MODE_SEQUENTIAL = 256
      integer, parameter :: YogiMPI_MODE_APPEND = 128

! MPI seeking constants
      integer, parameter :: YogiMPI_SEEK_SET = 50
      integer, parameter :: YogiMPI_SEEK_CUR = 55
      integer, parameter :: YogiMPI_SEEK_END = 60
  
! MPI_Offset need an INTEGER*8 value.
      integer, parameter :: YogiMPI_OFFSET_KIND = 8

      real*8 :: YOGIMPI_WTIME
      external YogiMPI_WTIME
