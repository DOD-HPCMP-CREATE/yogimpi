!
!                                  COPYRIGHT
! 
! The following is a notice of limited availability of the code, and disclaimer
! which must be included in the prologue of the code and in all source listings
! of the code.
! 
! Copyright Notice
!  + 2002 University of Chicago
!  + 2016 Stephen Adamec
!
! Permission is hereby granted to use, reproduce, prepare derivative works, and
! to redistribute to others.  This software was authored by:
! 
! Mathematics and Computer Science Division
! Argonne National Laboratory, Argonne IL 60439
! 
! (and)
! 
! Department of Computer Science
! University of Illinois at Urbana-Champaign
!
! (and)
!
! Stephen Adamec
! 
!                              GOVERNMENT LICENSE
! 
! Portions of this material resulted from work developed under a U.S.
! Government Contract and are subject to the following license: the Government
! is granted for itself and others acting on its behalf a paid-up, nonexclusive,
! irrevocable worldwide license in this computer software to reproduce, prepare
! derivative works, and perform publicly and display publicly.
!  
!                                    DISCLAIMER
!  
! This computer code material was prepared, in part, as an account of work
! sponsored by an agency of the United States Government.  Neither the United
! States, nor the University of Chicago, nor any of their employees, makes any
! warranty express or implied, or assumes any legal liability or responsibility
! for the accuracy, completeness, or usefulness of any information, apparatus,
! product, or process disclosed, or represents that its use would not infringe
! privately owned rights.
!

! Defined MPI constants for Fortran

! Version information
      integer, parameter :: YogiMPI_VERSION = 2
      integer, parameter :: YogiMPI_SUBVERSION = 2 

! Handle various byte counts in Fortran to match the C counterparts.  On most
! modern Intel/AMD 64-bit systems these are 4 or 8 byte values.
      integer, parameter :: YogiMPI_INTEGER_KIND = 4
      integer, parameter :: YogiMPI_OFFSET_KIND = 8
      integer, parameter :: YogiMPI_ADDRESS_KIND = 8

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

      integer, parameter :: YogiMPI_ERR_FILE = 20
      integer, parameter :: YogiMPI_ERR_NOT_SAME = 21
      integer, parameter :: YogiMPI_ERR_AMODE = 22
      integer, parameter :: YogiMPI_ERR_UNSUPPORTED_DATAREP = 23
      integer, parameter :: YogiMPI_ERR_UNSUPPORTED_OPERATION = 24
      integer, parameter :: YogiMPI_ERR_NO_SUCH_FILE = 25
      integer, parameter :: YogiMPI_ERR_FILE_EXISTS = 26
      integer, parameter :: YogiMPI_ERR_BAD_FILE = 27
      integer, parameter :: YogiMPI_ERR_ACCESS = 28
      integer, parameter :: YogiMPI_ERR_NO_SPACE = 29
      integer, parameter :: YogiMPI_ERR_QUOTA = 30
      integer, parameter :: YogiMPI_ERR_READ_ONLY = 31
      integer, parameter :: YogiMPI_ERR_FILE_IN_USE = 32
      integer, parameter :: YogiMPI_ERR_DUP_DATAREP = 33
      integer, parameter :: YogiMPI_ERR_CONVERSION = 34
      integer, parameter :: YogiMPI_ERR_IO = 35
      integer, parameter :: YogiMPI_ERR_LASTCODE = 36

! Special case with MPI_BOTTOM
      integer(YogiMPI_INTEGER_KIND) :: YogiMPI_BOTTOM

! assorted constants (both C and Fortran)
      integer, parameter :: YogiMPI_PROC_NULL = -2
      integer, parameter :: YogiMPI_ROOT = -6
      integer, parameter :: YogiMPI_ANY_SOURCE = -3
      integer, parameter :: YogiMPI_ANY_TAG = -4
      integer, parameter :: YogiMPI_UNDEFINED = -5
      integer, parameter :: YogiMPI_BSEND_OVERHEAD = 512 
      integer, parameter :: YogiMPI_KEYVAL_INVALID = -7

! Graph creation constants.  These are pointer values in C, so they must go
! within a common block.
      integer(YogiMPI_INTEGER_KIND) :: YogiMPI_WEIGHTS_EMPTY
      integer(YogiMPI_INTEGER_KIND) :: YogiMPI_UNWEIGHTED

! MPI asserts for one-sided communication.  Supports bitwise ops.
      integer, parameter :: YogiMPI_MODE_NOCHECK = 1024
      integer, parameter :: YogiMPI_MODE_NOSTORE = 2048
      integer, parameter :: YogiMPI_MODE_NOPUT = 4096
      integer, parameter :: YogiMPI_MODE_NOPRECEDE = 8192
      integer, parameter :: YogiMPI_MODE_NOSUCCEED = 16384

! Seeking constants.
      integer, parameter :: YogiMPI_SEEK_SET = 50
      integer, parameter :: YogiMPI_SEEK_CUR = 55
      integer, parameter :: YogiMPI_SEEK_END = 60

! Typeclasses
      integer, parameter :: YogiMPI_TYPECLASS_REAL = 1
      integer, parameter :: YogiMPI_TYPECLASS_INTEGER = 2
      integer, parameter :: YogiMPI_TYPECLASS_COMPLEX = 3

! MPI I/O permission modes.  Supports bitwise ops.
      integer, parameter :: YogiMPI_MODE_RDONLY = 2
      integer, parameter :: YogiMPI_MODE_RDWR = 8
      integer, parameter :: YogiMPI_MODE_WRONLY = 4
      integer, parameter :: YogiMPI_MODE_CREATE = 1
      integer, parameter :: YogiMPI_MODE_EXCL = 64
      integer, parameter :: YogiMPI_MODE_DELETE_ON_CLOSE = 16
      integer, parameter :: YogiMPI_MODE_UNIQUE_OPEN = 32
      integer, parameter :: YogiMPI_MODE_SEQUENTIAL = 256
      integer, parameter :: YogiMPI_MODE_APPEND = 128

! MPI_Info have key and value length constants.  Pick conservative values
! that accommodate OpenMPI and MPICH derivatives.
      integer, parameter :: YogiMPI_MAX_INFO_KEY = 32
      integer, parameter :: YogiMPI_MAX_INFO_VAL = 256

! Subarray and darray constructor constants.  Mimics the MPICH header file.
      integer, parameter :: YogiMPI_ORDER_C = 56
      integer, parameter :: YogiMPI_ORDER_FORTRAN = 57
      integer, parameter :: YogiMPI_DISTRIBUTE_BLOCK = 121
      integer, parameter :: YogiMPI_DISTRIBUTE_CYCLIC = 122
      integer, parameter :: YogiMPI_DISTRIBUTE_NONE = 123
      integer, parameter :: YogiMPI_DISTRIBUTE_DFLT_DARG = -49767

! Maximum sizes for strings
! This has to be one less than the value in C to account for null-terminated
! strings.
      integer, parameter :: YogiMPI_MAX_PROCESSOR_NAME = 31
      integer, parameter :: YogiMPI_MAX_ERROR_STRING = 31
      integer, parameter :: YogiMPI_MAX_PORT_NAME = 511
      integer, parameter :: YogiMPI_MAX_OBJECT_NAME = 63


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

! Special datatypes for constructing derived datatypes 
      integer, parameter :: YogiMPI_LB = 36 
      integer, parameter :: YogiMPI_UB = 37

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
      integer, parameter :: YogiMPI_DATATYPE_NULL = 0
      integer, parameter :: YogiMPI_REQUEST_NULL = 0
      integer, parameter :: YogiMPI_OP_NULL = 0 
      integer, parameter :: YogiMPI_INFO_NULL = 0
      integer, parameter :: YogiMPI_FILE_NULL = 0
      integer, parameter :: YogiMPI_ERRHANDLER_NULL = 0
      integer, parameter :: YogiMPI_WIN_NULL = 0

! Predefined error handlers
      integer, parameter :: YogiMPI_ERRORS_ARE_FATAL = 1
      integer, parameter :: YogiMPI_ERRORS_RETURN = 2 

! Empty group
      integer, parameter :: YogiMPI_GROUP_EMPTY = 1

! For supported thread levels
      integer, parameter :: YogiMPI_THREAD_SINGLE = 0
      integer, parameter :: YogiMPI_THREAD_FUNNELED = 1
      integer, parameter :: YogiMPI_THREAD_SERIALIZED = 2
      integer, parameter :: YogiMPI_THREAD_MULTIPLE = 3

! Lock type constants
      integer, parameter :: YogiMPI_LOCK_EXCLUSIVE = 234
      integer, parameter :: YogiMPI_LOCK_SHARED = 235

! Topology test constants 
      integer, parameter :: YogiMPI_GRAPH = 1
      integer, parameter :: YogiMPI_CART = 2

! MPI Combiner state constants
      integer, parameter :: YogiMPI_COMBINER_NAMED = 1
      integer, parameter :: YogiMPI_COMBINER_DUP = 2
      integer, parameter :: YogiMPI_COMBINER_CONTIGUOUS = 3
      integer, parameter :: YogiMPI_COMBINER_VECTOR = 4
      integer, parameter :: YogiMPI_COMBINER_HVECTOR_INTEGER = 5
      integer, parameter :: YogiMPI_COMBINER_HVECTOR = 6
      integer, parameter :: YogiMPI_COMBINER_INDEXED = 7
      integer, parameter :: YogiMPI_COMBINER_HINDEXED_INTEGER = 8
      integer, parameter :: YogiMPI_COMBINER_HINDEXED = 9
      integer, parameter :: YogiMPI_COMBINER_INDEXED_BLOCK = 10
      integer, parameter :: YogiMPI_COMBINER_STRUCT_INTEGER = 12
      integer, parameter :: YogiMPI_COMBINER_STRUCT = 13
      integer, parameter :: YogiMPI_COMBINER_SUBARRAY = 14
      integer, parameter :: YogiMPI_COMBINER_DARRAY = 15
      integer, parameter :: YogiMPI_COMBINER_F90_REAL = 16
      integer, parameter :: YogiMPI_COMBINER_F90_COMPLEX = 17
      integer, parameter :: YogiMPI_COMBINER_F90_INTEGER = 18
      integer, parameter :: YogiMPI_COMBINER_RESIZED = 19

! Predefined type for MPI_Comm_split_type
      integer, parameter :: YogiMPI_COMM_TYPE_SHARED = 1

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

! MPI_IN_PLACE is a special constant that must go in a common block.
      integer(YogiMPI_INTEGER_KIND) :: YogiMPI_IN_PLACE

! Declare common blocks.

      COMMON /ympi1/ YogiMPI_STATUS_IGNORE, YogiMPI_STATUSES_IGNORE
      COMMON /ympi2/ YogiMPI_IN_PLACE, YogiMPI_BOTTOM
      COMMON /ympi3/ YogiMPI_UNWEIGHTED, YogiMPI_WEIGHTS_EMPTY 

      real*8 :: YOGIBRIDGE_MPI_WTIME
      real*8 :: YOGIBRIDGE_MPI_WTICK
      external YogiBRIDGE_MPI_WTIME
      external YogiBRIDGE_MPI_WTICK
