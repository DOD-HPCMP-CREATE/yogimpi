/*
YogiMPI Library - MPI ABI Translator
Copyright (C) 2006, 2007 Toon Knapen Free Field Technologies S.A.
Additions made by Stephen Adamec, University of Alabama at Birmingham

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "yogimpi.h"

#ifndef MPITOYOGI_H
#define MPITOYOGI_H 

#define MPI_VERSION YogiMPI_VERSION
#define MPI_SUBVERSION YogiMPI_SUBVERSION

#define MPI_Group YogiMPI_Group
#define MPI_Comm YogiMPI_Comm
#define MPI_Datatype YogiMPI_Datatype
#define MPI_Request YogiMPI_Request
#define MPI_Op YogiMPI_Op
#define MPI_Info YogiMPI_Info
#define MPI_File YogiMPI_File
#define MPI_Offset YogiMPI_Offset
#define MPI_Errhandler YogiMPI_Errhandler

#define MPI_MAX_ERROR_STRING YogiMPI_MAX_ERROR_STRING
#define MPI_MAX_INFO_KEY YogiMPI_MAX_INFO_KEY
#define MPI_MAX_INFO_VAL YogiMPI_MAX_INFO_VAL 

#define MPI_ORDER_C YogiMPI_ORDER_C
#define MPI_ORDER_FORTRAN YogiMPI_ORDER_FORTRAN
#define MPI_DISTRIBUTE_BLOCK YogiMPI_DISTRIBUTE_BLOCK
#define MPI_DISTRIBUTE_CYCLIC YogiMPI_DISTRIBUTE_CYCLIC
#define MPI_DISTRIBUTE_NONE YogiMPI_DISTRIBUTE_NONE
#define MPI_DISTRIBUTE_DFLT_DARG YogiMPI_DISTRIBUTE_DFLT_DARG

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

#define MPI_ERR_FILE YogiMPI_ERR_FILE
#define MPI_ERR_NOT_SAME YogiMPI_ERR_NOT_SAME
#define MPI_ERR_AMODE YogiMPI_ERR_AMODE
#define MPI_ERR_UNSUPPORTED_DATAREP YogiMPI_ERR_UNSUPPORTED_DATAREP
#define MPI_ERR_UNSUPPORTED_OPERATION YogiMPI_ERR_UNSUPPORTED_OPERATION
#define MPI_ERR_NO_SUCH_FILE YogiMPI_ERR_NO_SUCH_FILE
#define MPI_ERR_FILE_EXISTS YogiMPI_ERR_FILE_EXISTS
#define MPI_ERR_BAD_FILE YogiMPI_ERR_BAD_FILE
#define MPI_ERR_ACCESS YogiMPI_ERR_ACCESS
#define MPI_ERR_NO_SPACE YogiMPI_ERR_NO_SPACE
#define MPI_ERR_QUOTA YogiMPI_ERR_QUOTA
#define MPI_ERR_READ_ONLY YogiMPI_ERR_READ_ONLY
#define MPI_ERR_FILE_IN_USE YogiMPI_ERR_FILE_IN_USE
#define MPI_ERR_DUP_DATAREP YogiMPI_ERR_DUP_DATAREP
#define MPI_ERR_CONVERSION YogiMPI_ERR_CONVERSION
#define MPI_ERR_IO YogiMPI_ERR_IO


#define MPI_BOTTOM YogiMPI_BOTTOM
#define MPI_PROC_NULL YogiMPI_PROC_NULL
#define MPI_ANY_SOURCE YogiMPI_ANY_SOURCE
#define MPI_ROOT YogiMPI_ROOT
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
#define MPI_SIGNED_CHAR YogiMPI_SIGNED_CHAR

#define MPI_LONG_LONG_INT YogiMPI_LONG_LONG_INT
#define MPI_INT32_T YogiMPI_INT32_T
#define MPI_INT64_T YogiMPI_INT64_T

#define MPI_LONG_LONG YogiMPI_LONG_LONG
#define MPI_COMPLEX YogiMPI_COMPLEX
#define MPI_DOUBLE_COMPLEX YogiMPI_DOUBLE_COMPLEX
#define MPI_LOGICAL YogiMPI_LOGICAL
#define MPI_2REAL YogiMPI_2REAL
#define MPI_2DOUBLE_PRECISION YogiMPI_2DOUBLE_PRECISION
#define MPI_2INTEGER YogiMPI_2INTEGER
#define MPI_INTEGER1 YogiMPI_INTEGER1
#define MPI_INTEGER2 YogiMPI_INTEGER2
#define MPI_INTEGER4 YogiMPI_INTEGER4
#define MPI_INTEGER8 YogiMPI_INTEGER8
#define MPI_REAL YogiMPI_REAL
#define MPI_REAL4 YogiMPI_REAL4
#define MPI_REAL8 YogiMPI_REAL8
#define MPI_UNSIGNED_LONG_LONG YogiMPI_UNSIGNED_LONG_LONG
#define MPI_CHARACTER YogiMPI_CHARACTER
#define MPI_INTEGER YogiMPI_INTEGER
#define MPI_DOUBLE_PRECISION YogiMPI_DOUBLE_PRECISION

#define MPI_UB YogiMPI_UB
#define MPI_LB YogiMPI_LB

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
#define MPI_ERRHANDLER_NULL YogiMPI_ERRHANDLER_NULL

#define MPI_ERRORS_ARE_FATAL YogiMPI_ERRORS_ARE_FATAL
#define MPI_ERRORS_RETURN YogiMPI_ERRORS_RETURN

#define MPI_GROUP_EMPTY YogiMPI_GROUP_EMPTY

#define MPI_Status YogiMPI_Status
#define MPI_STATUS_IGNORE YogiMPI_STATUS_IGNORE
#define MPI_STATUSES_IGNORE YogiMPI_STATUSES_IGNORE

#define MPI_Aint YogiMPI_Aint

#define MPI_Fint YogiMPI_Fint

#define MPI_IN_PLACE YogiMPI_IN_PLACE

#define MPI_COMBINER_NAMED YogiMPI_COMBINER_NAMED
#define MPI_COMBINER_DUP YogiMPI_COMBINER_DUP
#define MPI_COMBINER_CONTIGUOUS YogiMPI_COMBINER_CONTIGUOUS
#define MPI_COMBINER_VECTOR YogiMPI_COMBINER_VECTOR
#define MPI_COMBINER_HVECTOR_INTEGER YogiMPI_COMBINER_HVECTOR_INTEGER
#define MPI_COMBINER_HVECTOR YogiMPI_COMBINER_HVECTOR
#define MPI_COMBINER_INDEXED YogiMPI_COMBINER_INDEXED
#define MPI_COMBINER_HINDEXED_INTEGER YogiMPI_COMBINER_HINDEXED_INTEGER
#define MPI_COMBINER_HINDEXED YogiMPI_COMBINER_HINDEXED
#define MPI_COMBINER_INDEXED_BLOCK YogiMPI_COMBINER_INDEXED_BLOCK
#define MPI_COMBINER_STRUCT_INTEGER YogiMPI_COMBINER_STRUCT_INTEGER
#define MPI_COMBINER_STRUCT YogiMPI_COMBINER_STRUCT
#define MPI_COMBINER_SUBARRAY YogiMPI_COMBINER_SUBARRAY
#define MPI_COMBINER_DARRAY YogiMPI_COMBINER_DARRAY
#define MPI_COMBINER_F90_REAL YogiMPI_COMBINER_F90_REAL
#define MPI_COMBINER_F90_COMPLEX YogiMPI_COMBINER_F90_COMPLEX
#define MPI_COMBINER_F90_INTEGER YogiMPI_COMBINER_F90_INTEGER
#define MPI_COMBINER_RESIZED YogiMPI_COMBINER_RESIZED

#define MPI_User_function YogiMPI_User_function
#define MPI_Copy_function YogiMPI_Copy_function
#define MPI_Delete_function YogiMPI_Delete_function
#define MPI_Comm_copy_attr_function YogiMPI_Comm_copy_attr_function
#define MPI_Comm_delete_attr_function YogiMPI_Comm_delete_attr_function

#define MPI_COMM_NULL_COPY_FN YogiMPI_COMM_NULL_COPY_FN
#define MPI_COMM_DUP_FN YogiMPI_COMM_DUP_FN
#define MPI_COMM_NULL_DELETE_FN YogiMPI_COMM_NULL_DELETE_FN
#define MPI_NULL_COPY_FN YogiMPI_NULL_COPY_FN
#define MPI_DUP_FN YogiMPI_DUP_FN
#define MPI_NULL_DELETE_FN YogiMPI_NULL_DELETE_FN

#define MPI_Send YogiMPI_Send
#define MPI_Recv YogiMPI_Recv
#define MPI_Get_count YogiMPI_Get_count
#define MPI_Ssend YogiMPI_Ssend
#define MPI_Isend YogiMPI_Isend
#define MPI_Issend YogiMPI_Issend
#define MPI_Irecv YogiMPI_Irecv
#define MPI_Wait YogiMPI_Wait
#define MPI_Request_free YogiMPI_Request_free
#define MPI_Waitsome YogiMPI_Waitsome
#define MPI_Waitany YogiMPI_Waitany
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
#define MPI_Comm_create YogiMPI_Comm_create
#define MPI_Comm_group YogiMPI_Comm_group
#define MPI_Comm_size YogiMPI_Comm_size
#define MPI_Comm_rank YogiMPI_Comm_rank
#define MPI_Comm_dup YogiMPI_Comm_dup
#define MPI_Comm_split YogiMPI_Comm_split
#define MPI_Comm_free YogiMPI_Comm_free
#define MPI_Group_free YogiMPI_Group_free
#define MPI_Group_incl YogiMPI_Group_incl
#define MPI_Group_rank YogiMPI_Group_rank
#define MPI_Group_translate_ranks YogiMPI_Group_translate_ranks
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
#define MPI_File_get_position YogiMPI_File_get_position
#define MPI_Info_create YogiMPI_Info_create
#define MPI_Info_set YogiMPI_Info_set
#define MPI_Test YogiMPI_Test
#define MPI_Probe YogiMPI_Probe
#define MPI_Iprobe YogiMPI_Iprobe
#define MPI_Abort YogiMPI_Abort
#define MPI_Attr_get YogiMPI_Attr_get
#define MPI_Finalized YogiMPI_Finalized
#define MPI_Initialized YogiMPI_Initialized
#define MPI_Info_get_nthkey YogiMPI_Info_get_nthkey
#define MPI_File_write YogiMPI_File_write
#define MPI_Info_dup YogiMPI_Info_dup
#define MPI_File_read_at_all YogiMPI_File_read_at_all
#define MPI_File_seek YogiMPI_File_seek
#define MPI_Type_struct YogiMPI_Type_struct
#define MPI_File_read_at YogiMPI_File_read_at
#define MPI_File_write_at_all YogiMPI_File_write_at_all
#define MPI_Type_extent YogiMPI_Type_extent
#define MPI_Get_elements YogiMPI_Get_elements
#define MPI_Type_hindexed YogiMPI_Type_hindexed
#define MPI_File_set_atomicity YogiMPI_File_set_atomicity
#define MPI_File_sync YogiMPI_File_sync
#define MPI_Type_hvector YogiMPI_Type_hvector
#define MPI_File_delete YogiMPI_File_delete
#define MPI_Address YogiMPI_Address
#define MPI_Info_free YogiMPI_Info_free
#define MPI_Info_get YogiMPI_Info_get
#define MPI_Group_excl YogiMPI_Group_excl
#define MPI_File_get_atomicity YogiMPI_File_get_atomicity
#define MPI_File_read YogiMPI_File_read
#define MPI_Info_get_nkeys YogiMPI_Info_get_nkeys
#define MPI_File_get_size YogiMPI_File_get_size
#define MPI_File_set_size YogiMPI_File_set_size
#define MPI_Error_string YogiMPI_Error_string
#define MPI_Comm_f2c YogiMPI_Comm_f2c
#define MPI_Comm_c2f YogiMPI_Comm_c2f
#define MPI_Info_f2c YogiMPI_Info_f2c
#define MPI_Info_c2f YogiMPI_Info_c2f
#define MPI_Type_f2c YogiMPI_Type_f2c
#define MPI_File_read_all YogiMPI_File_read_all
#define MPI_Type_get_extent YogiMPI_Type_get_extent
#define MPI_Cancel YogiMPI_Cancel
#define MPI_Attr_put YogiMPI_Attr_put
#define MPI_Buffer_attach YogiMPI_Buffer_attach
#define MPI_Rsend YogiMPI_Rsend
#define MPI_Unpack YogiMPI_Unpack
#define MPI_Buffer_detach YogiMPI_Buffer_detach
#define MPI_Bsend YogiMPI_Bsend
#define MPI_Pack_size YogiMPI_Pack_size
#define MPI_Intercomm_create YogiMPI_Intercomm_create
#define MPI_Keyval_free YogiMPI_Keyval_free
#define MPI_Pack YogiMPI_Pack
#define MPI_Group_range_excl YogiMPI_Group_range_excl
#define MPI_Type_create_hindexed YogiMPI_Type_create_hindexed
#define MPI_Pcontrol YogiMPI_Pcontrol
#define MPI_Error_class YogiMPI_Error_class
#define MPI_Irsend YogiMPI_Irsend
#define MPI_Wtick YogiMPI_Wtick
#define MPI_Type_create_struct YogiMPI_Type_create_struct
#define MPI_Reduce_scatter YogiMPI_Reduce_scatter
#define MPI_Comm_compare YogiMPI_Comm_compare
#define MPI_Sendrecv YogiMPI_Sendrecv
#define MPI_Group_size YogiMPI_Group_size
#define MPI_Type_create_subarray YogiMPI_Type_create_subarray
#define MPI_Attr_delete YogiMPI_Attr_delete
#define MPI_Comm_remote_size YogiMPI_Comm_remote_size
#define MPI_Get_address YogiMPI_Get_address
#define MPI_Type_create_hvector YogiMPI_Type_create_hvector
#define MPI_Errhandler_free YogiMPI_Errhandler_free
#define MPI_Comm_get_errhandler YogiMPI_Comm_get_errhandler
#define MPI_Comm_set_errhandler YogiMPI_Comm_set_errhandler
#define MPI_Errhandler_set YogiMPI_Errhandler_set
#define MPI_Type_get_envelope YogiMPI_Type_get_envelope
#define MPI_Type_get_contents YogiMPI_Type_get_contents
#define MPI_Op_free YogiMPI_Op_free
#define MPI_Cart_coords YogiMPI_Cart_coords
#define MPI_Cart_create YogiMPI_Cart_create
#define MPI_Dims_create YogiMPI_Dims_create
#define MPI_Info_get_valuelen YogiMPI_Info_get_valuelen
#define MPI_Type_create_darray YogiMPI_Type_create_darray
#define MPI_Type_create_resized YogiMPI_Type_create_resized
#define MPI_Keyval_create YogiMPI_Keyval_create
#define MPI_Comm_create_errhandler YogiMPI_Comm_create_errhandler
#define MPI_Op_create YogiMPI_Op_create
#define MPI_Comm_call_errhandler YogiMPI_Comm_call_errhandler
#define MPI_Testall YogiMPI_Testall
#define MPI_Test_cancelled YogiMPI_Test_cancelled
#define MPI_Group_range_incl YogiMPI_Group_range_incl
#define MPI_Testany YogiMPI_Testany
#define MPI_Testsome YogiMPI_Testsome
#define MPI_Comm_set_attr YogiMPI_Comm_set_attr
#define MPI_Comm_create_keyval YogiMPI_Comm_create_keyval
#define MPI_Comm_free_keyval YogiMPI_Comm_free_keyval
#define MPI_Comm_delete_attr YogiMPI_Comm_delete_attr
#define MPI_Comm_get_attr YogiMPI_Comm_get_attr
#define MPI_Type_dup YogiMPI_Type_dup
#define MPI_Type_get_true_extent YogiMPI_Type_get_true_extent
#define MPI_Alloc_mem YogiMPI_Alloc_mem
#define MPI_Free_mem YogiMPI_Free_mem
#define MPI_Exscan YogiMPI_Exscan
#define MPI_Comm_test_inter YogiMPI_Comm_test_inter
#define MPI_Iallreduce YogiMPI_Iallreduce

#endif /* MPITOYOGI_H */
