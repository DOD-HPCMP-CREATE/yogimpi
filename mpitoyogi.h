#include "yogimpi.h"

#ifndef MPITOYOGI_H
#define MPITOYOGI_H 

#define MPI_Group YogiMPI_Group
#define MPI_Comm YogiMPI_Comm
#define MPI_Datatype YogiMPI_Datatype
#define MPI_Request YogiMPI_Request
#define MPI_Op YogiMPI_Op
#define MPI_Info YogiMPI_Info
#define MPI_File YogiMPI_File

#define MPI_SUCCESS YogiMPI_SUCCESS
#define MPI_ERR_BUFFER YogiMPI_ERR_BUFFER
#define MPI_ERR_COUNT YogiMPI_ERR_COUNT
#define MPI_ERR_TYPE YogiMPI_ERR_TYPE
#define MPI_ERR_TAG YogiMPI_ERR_TAG
#define MPI_ERR_COMM YogiMPI_ERR_COMM
#define MPI_ERR_RANK YogiMPI_ERR_RANK
#define MPI_ERR_REQUEST YogiMPI_ERR_REQUEST
#define MPI_ERR_ROOT YogiMPI_ERR_ROOT
#define MPI_ERR_GROUP YogiMPI_ERR_GROUP
#define MPI_ERR_OP YogiMPI_ERR_OP
#define MPI_ERR_TOPOLOGY YogiMPI_ERR_TOPOLOGY
#define MPI_ERR_DIMS YogiMPI_ERR_DIMS
#define MPI_ERR_ARG YogiMPI_ERR_ARG
#define MPI_ERR_UNKNOWN YogiMPI_ERR_UNKNOWN
#define MPI_ERR_TRUNCATE YogiMPI_ERR_TRUNCATE
#define MPI_ERR_OTHER YogiMPI_ERR_OTHER
#define MPI_ERR_INTERN YogiMPI_ERR_INTERN
#define MPI_ERR_PENDING YogiMPI_ERR_PENDING
#define MPI_ERR_IN_STATUS YogiMPI_ERR_IN_STATUS
#define MPI_ERR_LASTCODE YogiMPI_ERR_LASTCODE

#define MPI_BOTTOM YogiMPI_BOTTOM
#define MPI_PROC_NULL YogiMPI_PROC_NULL
#define MPI_ANY_SOURCE YogiMPI_ANY_SOURCE
#define MPI_ANY_TAG YogiMPI_ANY_TAG
#define MPI_UNDEFINED YogiMPI_UNDEFINED
#define MPI_BSEND_OVERHEAD YogiMPI_BSEND_OVERHEAD
#define MPI_KEYVAL_INVALID YogiMPI_KEYVAL_INVALID

#define MPI_MODE_RDONLY YogiMPI_MODE_RDONLY
#define MPI_MODE_RDWR YogiMPI_MODE_RDWR
#define MPI_MODE_WRONLY YogiMPI_MODE_WRONLY
#define MPI_MODE_CREATE YogiMPI_MODE_CREATE
#define MPI_MODE_EXCL YogiMPI_MODE_EXCL
#define MPI_MODE_DELETE_ON_CLOSE YogiMPI_MODE_DELETE_ON_CLOSE
#define MPI_MODE_UNIQUE_OPEN YogiMPI_MODE_UNIQUE_OPEN
#define MPI_MODE_SEQUENTIAL YogiMPI_MODE_SEQUENTIAL
#define MPI_MODE_APPEND YogiMPI_MODE_APPEND

#define MPI_MAX_PROCESSOR_NAME YogiMPI_MAX_PROCESSOR_NAME

#define MPI_CHAR YogiMPI_CHAR
#define MPI_SHORT YogiMPI_SHORT
#define MPI_INT YogiMPI_INT
#define MPI_LONG YogiMPI_LONG
#define MPI_UNSIGNED_CHAR YogiMPI_UNSIGNED_CHAR
#define MPI_UNSIGNED_SHORT YogiMPI_UNSIGNED_SHORT
#define MPI_UNSIGNED YogiMPI_UNSIGNED
#define MPI_UNSIGNED_LONG YogiMPI_UNSIGNED_LONG
#define MPI_FLOAT YogiMPI_FLOAT
#define MPI_DOUBLE YogiMPI_DOUBLE
#define MPI_LONG_DOUBLE YogiMPI_LONG_DOUBLE
#define MPI_BYTE YogiMPI_BYTE
#define MPI_PACKED YogiMPI_PACKED
#define MPI_FLOAT_INT YogiMPI_FLOAT_INT
#define MPI_DOUBLE_INT YogiMPI_DOUBLE_INT
#define MPI_LONG_INT YogiMPI_LONG_INT
#define MPI_2INT YogiMPI_2INT
#define MPI_SHORT_INT YogiMPI_SHORT_INT
#define MPI_LONG_DOUBLE_INT YogiMPI_LONG_DOUBLE_INT

#define MPI_LONG_LONG_INT YogiMPI_LONG_LONG_INT
#define MPI_INT32_T YogiMPI_INT32_T
#define MPI_INT64_T YogiMPI_INT64_T

#define MPI_COMM_WORLD YogiMPI_COMM_WORLD
#define MPI_COMM_SELF YogiMPI_COMM_SELF

#define MPI_IDENT YogiMPI_IDENT
#define MPI_CONGRUENT YogiMPI_CONGRUENT
#define MPI_SIMILAR YogiMPI_SIMILAR
#define MPI_UNEQUAL YogiMPI_UNEQUAL

#define MPI_TAG_UB YogiMPI_TAG_UB
#define MPI_IO YogiMPI_IO
#define MPI_HOST YogiMPI_HOST
#define MPI_WTIME_IS_GLOBAL YogiMPI_WTIME_IS_GLOBAL

#define MPI_MAX YogiMPI_MAX
#define MPI_MIN YogiMPI_MIN
#define MPI_SUM YogiMPI_SUM
#define MPI_PROD YogiMPI_PROD
#define MPI_MAXLOC YogiMPI_MAXLOC
#define MPI_MINLOC YogiMPI_MINLOC
#define MPI_BAND YogiMPI_BAND
#define MPI_BOR YogiMPI_BOR
#define MPI_BXOR YogiMPI_BXOR
#define MPI_LAND YogiMPI_LAND
#define MPI_LOR YogiMPI_LOR
#define MPI_LXOR YogiMPI_LXOR

#define MPI_GROUP_NULL YogiMPI_GROUP_NULL 
#define MPI_COMM_NULL YogiMPI_COMM_NULL
#define MPI_DATATYPE_NULL YogiMPI_DATATYPE_NULL
#define MPI_REQUEST_NULL YogiMPI_REQUEST_NULL
#define MPI_OP_NULL YogiMPI_OP_NULL
#define MPI_INFO_NULL YogiMPI_INFO_NULL
#define MPI_FILE_NULL YogiMPI_FILE_NULL

#define MPI_GROUP_EMPTY YogiMPI_GROUP_EMPTY

#define MPI_Status YogiMPI_Status
#define MPI_STATUS_IGNORE YogiMPI_STATUS_IGNORE
#define MPI_STATUSES_IGNORE YogiMPI_STATUSES_IGNORE

#define MPI_Aint YogiMPI_Aint

#define MPI_Fint YogiMPI_Fint

#define MPI_IN_PLACE YogiMPI_IN_PLACE

#define MPI_Send YogiMPI_Send
#define MPI_Recv YogiMPI_Recv
#define MPI_Get_count YogiMPI_Get_count
#define MPI_Ssend YogiMPI_Ssend
#define MPI_Isend YogiMPI_Isend
#define MPI_Issend YogiMPI_Issend
#define MPI_Irecv YogiMPI_Irecv
#define MPI_Wait YogiMPI_Wait
#define MPI_Request_free YogiMPI_Request_free
#define MPI_Waitall YogiMPI_Waitall
#define MPI_Send_init YogiMPI_Send_init
#define MPI_Type_size YogiMPI_Type_size
#define MPI_Type_contiguous YogiMPI_Type_contiguous
#define MPI_Type_vector YogiMPI_Type_vector
#define MPI_Type_indexed YogiMPI_Type_indexed
#define MPI_Type_commit YogiMPI_Type_commit
#define MPI_Type_free YogiMPI_Type_free
#define MPI_Barrier YogiMPI_Barrier
#define MPI_Bcast YogiMPI_Bcast
#define MPI_Gather YogiMPI_Gather
#define MPI_Gatherv YogiMPI_Gatherv
#define MPI_Scatter YogiMPI_Scatter
#define MPI_Scatterv YogiMPI_Scatterv
#define MPI_Allgather YogiMPI_Allgather
#define MPI_Allgatherv YogiMPI_Allgatherv
#define MPI_Reduce YogiMPI_Reduce
#define MPI_Allreduce YogiMPI_Allreduce
#define MPI_Comm_size YogiMPI_Comm_size
#define MPI_Comm_rank YogiMPI_Comm_rank
#define MPI_Comm_dup YogiMPI_Comm_dup
#define MPI_Comm_split YogiMPI_Comm_split
#define MPI_Comm_free YogiMPI_Comm_free
#define MPI_Get_processor_name YogiMPI_Get_processor_name
#define MPI_Wtime YogiMPI_Wtime
#define MPI_Init YogiMPI_Init
#define MPI_Finalize YogiMPI_Finalize
#define MPI_Type_create_indexed_block YogiMPI_Type_create_indexed_block
#define MPI_Comm_f2c YogiMPI_Comm_f2c
#define MPI_Recv_init YogiMPI_Recv_init
#define MPI_Scan YogiMPI_Scan
#define MPI_Startall YogiMPI_Startall
#define MPI_Alltoall YogiMPI_Alltoall
#define MPI_Alltoallv YogiMPI_Alltoallv
#define MPI_File_close YogiMPI_File_close
#define MPI_File_get_info YogiMPI_File_get_info
#define MPI_File_open YogiMPI_File_open
#define MPI_File_set_view YogiMPI_File_set_view
#define MPI_File_write_all YogiMPI_File_write_all
#define MPI_File_write_at YogiMPI_File_write_at
#define MPI_Info_create YogiMPI_Info_create
#define MPI_Info_set YogiMPI_Info_set
#define MPI_Test YogiMPI_Test
#define MPI_Iprobe YogiMPI_Iprobe
#define MPI_Abort YogiMPI_Abort

#endif /* MPITOYOGI_H */
