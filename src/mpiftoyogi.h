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

include 'yogimpif.h'

#define MPI_SUCCESS YogiMPI_SUCCESS
#define MPI_ERR_BUFFER YogiMPI_ERR_BUFFER
#define MPI_ERR_COUNT YogiMPI_ERR_COUNT 
#define MPI_ERR_TYPE YogiMPI_ERR_TYPE
#define MPI_ERR_TAG YogiMPI_ERR_TAG
#define MPI_ERR_COMM YogiMPI_ERR_COMM
#define MPI_ERR_RANK YogiMPI_ERR_RANK
#define MPI_ERR_REQUEST  YogiMPI_ERR_REQUEST 
#define MPI_ERR_ROOT YogiMPI_ERR_ROOT
#define MPI_ERR_GROUP YogiMPI_ERR_GROUP 
#define MPI_ERR_OP YogiMPI_ERR_OP 
#define MPI_ERR_TOPOLOGY YogiMPI_ERR_TOPOLOGY 
#define MPI_ERR_DIMS YogiMPI_ERR_DIMS 
#define MPI_ERR_ARG  YogiMPI_ERR_ARG 
#define MPI_ERR_UNKNOWN YogiMPI_ERR_UNKNOWN
#define MPI_ERR_TRUNCATE YogiMPI_ERR_TRUNCATE
#define MPI_ERR_OTHER YogiMPI_ERR_OTHER
#define MPI_ERR_INTERN YogiMPI_ERR_INTERN
#define MPI_ERR_PENDING YogiMPI_ERR_PENDING
#define MPI_ERR_IN_STATUS YogiMPI_ERR_IN_STATUS 
#define MPI_ERR_LASTCODE YogiMPI_ERR_LASTCODE

! assorted constants (both C and Fortran)
#define MPI_BOTTOM YogiMPI_BOTTOM
#define MPI_PROC_NULL YogiMPI_PROC_NULL
#define MPI_ANY_SOURCE YogiMPI_ANY_SOURCE
#define MPI_ANY_TAG YogiMPI_ANY_TAG
#define MPI_UNDEFINED YogiMPI_UNDEFINED
#define MPI_BSEND_OVERHEAD YogiMPI_BSEND_OVERHEAD
#define MPI_KEYVAL_INVALID YogiMPI_KEYVAL_INVALID

! status size and reserved index values (Fortran)
#define MPI_STATUS_SIZE YogiMPI_STATUS_SIZE
#define MPI_SOURCE YogiMPI_SOURCE
#define MPI_TAG YogiMPI_TAG 
#define MPI_ERROR YogiMPI_ERROR

! Maximum sizes for strings
! This has to be one less than the value in C to account for null-terminated
! strings.
#define MPI_MAX_PROCESSOR_NAME YogiMPI_MAX_PROCESSOR_NAME

! elementary datatypes (Fortran)
#define MPI_CHARACTER YogiMPI_CHARACTER
#define MPI_INTEGER YogiMPI_INTEGER
#define MPI_REAL YogiMPI_REAL
#define MPI_DOUBLE_PRECISION YogiMPI_DOUBLE_PRECISION
#define MPI_BYTE YogiMPI_BYTE
#define MPI_PACKED YogiMPI_PACKED
#define MPI_COMPLEX YogiMPI_COMPLEX
#define MPI_DOUBLE_COMPLEX YogiMPI_DOUBLE_COMPLEX
#define MPI_LOGICAL YogiMPI_LOGICAL

! datatypes for reduction functions (Fortran)
#define MPI_2REAL YogiMPI_2REAL
#define MPI_2DOUBLE_PRECISION YogiMPI_2DOUBLE_PRECISION
#define MPI_2INTEGER YogiMPI_2INTEGER

! optional datatypes (Fortran)
#define MPI_INTEGER1 YogiMPI_INTEGER1
#define MPI_INTEGER2 YogiMPI_INTEGER2 
#define MPI_INTEGER4 YogiMPI_INTEGER4 
#define MPI_INTEGER8 YogiMPI_INTEGER8
#define MPI_REAL4 YogiMPI_REAL4
#define MPI_REAL8 YogiMPI_REAL8

! reserved communicators (C and Fortran)
#define MPI_COMM_WORLD YogiMPI_COMM_WORLD
#define MPI_COMM_SELF YogiMPI_COMM_SELF

! results of communicator and group comparisons
#define MPI_IDENT YogiMPI_IDENT
#define MPI_CONGRUENT YogiMPI_CONGRUENT
#define MPI_SIMILAR YogiMPI_SIMILAR
#define MPI_UNEQUAL YogiMPI_UNEQUAL

! environmental inquiry keys (C and Fortran)
#define MPI_TAG_UB YogiMPI_TAG_UB
#define MPI_IO YogiMPI_IO
#define MPI_HOST YogiMPI_HOST
#define MPI_WTIME_IS_GLOBAL YogiMPI_WTIME_IS_GLOBAL

! collective operations (C and Fortran)
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

! Null handles

#define MPI_GROUP_NULL YogiMPI_GROUP_NULL
#define MPI_COMM_NULL YogiMPI_COMM_NULL
#define MPI_REQUEST_NULL YogiMPI_REQUEST_NULL
#define MPI_DATATYPE_NULL YogiMPI_DATATYPE_NULL
#define MPI_INFO_NULL YogiMPI_INFO_NULL
#define MPI_FILE_NULL YogiMPI_FILE_NULL

#define MPI_GROUP_EMPTY YogiMPI_GROUP_EMPTY

#define MPI_MODE_RDONLY YogiMPI_MODE_RDONLY
#define MPI_MODE_RDWR YogiMPI_MODE_RDWR
#define MPI_MODE_WRONLY YogiMPI_MODE_WRONLY
#define MPI_MODE_CREATE YogiMPI_MODE_CREATE
#define MPI_MODE_EXCL YogiMPI_MODE_EXCL
#define MPI_MODE_DELETE_ON_CLOSE YogiMPI_MODE_DELETE_ON_CLOSE
#define MPI_MODE_UNIQUE_OPEN YogiMPI_MODE_UNIQUE_OPEN
#define MPI_MODE_SEQUENTIAL YogiMPI_MODE_SEQUENTIAL
#define MPI_MODE_APPEND YogiMPI_MODE_APPEND

#define MPI_OFFSET_KIND YogiMPI_OFFSET_KIND

#define MPI_STATUS_IGNORE YogiMPI_STATUS_IGNORE
#define MPI_STATUSES_IGNORE YogiMPI_STATUSES_IGNORE
#define MPI_WTIME YOGIMPI_WTIME

#define MPI_SEND YOGIMPI_SEND
#define MPI_RECV YOGIMPI_RECV
#define MPI_GET_COUNT YOGIMPI_GET_COUNT
#define MPI_SSEND YOGIMPI_SSEND
#define MPI_ISEND YOGIMPI_ISEND
#define MPI_ISSEND YOGIMPI_ISSEND
#define MPI_IRECV YOGIMPI_IRECV
#define MPI_WAIT YOGIMPI_WAIT
#define MPI_REQUEST_FREE YOGIMPI_REQUEST_FREE
#define MPI_WAITSOME YOGIMPI_WAITSOME
#define MPI_WAITANY YOGIMPI_WAITANY
#define MPI_WAITALL YOGIMPI_WAITALL
#define MPI_SEND_INIT YOGIMPI_SEND_INIT
#define MPI_TYPE_SIZE YOGIMPI_TYPE_SIZE
#define MPI_TYPE_CONTIGUOUS YOGIMPI_TYPE_CONTIGUOUS
#define MPI_TYPE_VECTOR YOGIMPI_TYPE_VECTOR
#define MPI_TYPE_INDEXED YOGIMPI_TYPE_INDEXED
#define MPI_TYPE_COMMIT YOGIMPI_TYPE_COMMIT
#define MPI_TYPE_FREE YOGIMPI_TYPE_FREE
#define MPI_BARRIER YOGIMPI_BARRIER
#define MPI_BCAST YOGIMPI_BCAST
#define MPI_GATHER YOGIMPI_GATHER
#define MPI_GATHERV YOGIMPI_GATHERV
#define MPI_SCATTER YOGIMPI_SCATTER
#define MPI_SCATTERV YOGIMPI_SCATTERV
#define MPI_ALLGATHER YOGIMPI_ALLGATHER
#define MPI_ALLGATHERV YOGIMPI_ALLGATHERV
#define MPI_REDUCE YOGIMPI_REDUCE
#define MPI_ALLREDUCE YOGIMPI_ALLREDUCE
#define MPI_COMM_CREATE YOGIMPI_COMM_CREATE
#define MPI_COMM_GROUP YOGIMPI_COMM_GROUP
#define MPI_COMM_SIZE YOGIMPI_COMM_SIZE
#define MPI_COMM_RANK YOGIMPI_COMM_RANK
#define MPI_COMM_DUP YOGIMPI_COMM_DUP
#define MPI_COMM_SPLIT YOGIMPI_COMM_SPLIT
#define MPI_COMM_FREE YOGIMPI_COMM_FREE
#define MPI_GROUP_FREE YOGIMPI_GROUP_FREE
#define MPI_GROUP_INCL YOGIMPI_GROUP_INCL
#define MPI_GROUP_RANK YOGIMPI_GROUP_RANK
#define MPI_GROUP_TRANSLATE_RANKS YOGIMPI_GROUP_TRANSLATE_RANKS
#define MPI_GET_PROCESSOR_NAME YOGIMPI_GET_PROCESSOR_NAME
#define MPI_WTIME YOGIMPI_WTIME
#define MPI_INIT YOGIMPI_INIT
#define MPI_FINALIZE YOGIMPI_FINALIZE
#define MPI_TYPE_CREATE_INDEXED_BLOCK YOGIMPI_TYPE_CREATE_INDEXED_BLOCK
#define MPI_COMM_F2C YOGIMPI_COMM_F2C
#define MPI_RECV_INIT YOGIMPI_RECV_INIT
#define MPI_SCAN YOGIMPI_SCAN
#define MPI_STARTALL YOGIMPI_STARTALL
#define MPI_ALLTOALL YOGIMPI_ALLTOALL
#define MPI_ALLTOALLV YOGIMPI_ALLTOALLV
#define MPI_FILE_CLOSE YOGIMPI_FILE_CLOSE
#define MPI_FILE_GET_INFO YOGIMPI_FILE_GET_INFO
#define MPI_FILE_OPEN YOGIMPI_FILE_OPEN
#define MPI_FILE_SET_VIEW YOGIMPI_FILE_SET_VIEW
#define MPI_FILE_WRITE_ALL YOGIMPI_FILE_WRITE_ALL
#define MPI_FILE_WRITE_AT YOGIMPI_FILE_WRITE_AT
#define MPI_INFO_CREATE YOGIMPI_INFO_CREATE
#define MPI_INFO_SET YOGIMPI_INFO_SET
#define MPI_TEST YOGIMPI_TEST
#define MPI_PROBE YOGIMPI_PROBE
#define MPI_IPROBE YOGIMPI_IPROBE
#define MPI_ABORT YOGIMPI_ABORT
#define MPI_ATTR_GET YOGIMPI_ATTR_GET
#define MPI_FINALIZED YOGIMPI_FINALIZED
#define MPI_INITIALIZED YOGIMPI_INITIALIZED

! And now the lowercase version since there is no case sensitivity.
! I love you, Fortran.

#define mpi_success yogimpi_success
#define mpi_err_buffer yogimpi_err_buffer
#define mpi_err_count yogimpi_err_count 
#define mpi_err_type yogimpi_err_type
#define mpi_err_tag yogimpi_err_tag
#define mpi_err_comm yogimpi_err_comm
#define mpi_err_rank yogimpi_err_rank
#define mpi_err_request  yogimpi_err_request 
#define mpi_err_root yogimpi_err_root
#define mpi_err_group yogimpi_err_group 
#define mpi_err_op yogimpi_err_op 
#define mpi_err_topology yogimpi_err_topology 
#define mpi_err_dims yogimpi_err_dims 
#define mpi_err_arg  yogimpi_err_arg 
#define mpi_err_unknown yogimpi_err_unknown
#define mpi_err_truncate yogimpi_err_truncate
#define mpi_err_other yogimpi_err_other
#define mpi_err_intern yogimpi_err_intern
#define mpi_err_pending yogimpi_err_pending
#define mpi_err_in_status yogimpi_err_in_status 
#define mpi_err_lastcode yogimpi_err_lastcode

! assorted constants (both c and fortran)
#define mpi_bottom yogimpi_bottom
#define mpi_proc_null yogimpi_proc_null
#define mpi_any_source yogimpi_any_source
#define mpi_any_tag yogimpi_any_tag
#define mpi_undefined yogimpi_undefined
#define mpi_bsend_overhead yogimpi_bsend_overhead
#define mpi_keyval_invalid yogimpi_keyval_invalid

! status size and reserved index values (fortran)
#define mpi_status_size yogimpi_status_size
#define mpi_source yogimpi_source
#define mpi_tag yogimpi_tag 
#define mpi_error yogimpi_error

! maximum sizes for strings
! this has to be one less than the value in c to account for null-terminated
! strings.
#define mpi_max_processor_name yogimpi_max_processor_name

! elementary datatypes (fortran)
#define mpi_character yogimpi_character
#define mpi_integer yogimpi_integer
#define mpi_real yogimpi_real
#define mpi_double_precision yogimpi_double_precision
#define mpi_byte yogimpi_byte
#define mpi_packed yogimpi_packed
#define mpi_complex yogimpi_complex
#define mpi_double_complex yogimpi_double_complex
#define mpi_logical yogimpi_logical

! datatypes for reduction functions (fortran)
#define mpi_2real yogimpi_2real
#define mpi_2double_precision yogimpi_2double_precision
#define mpi_2integer yogimpi_2integer

! optional datatypes (fortran)
#define mpi_integer1 yogimpi_integer1
#define mpi_integer2 yogimpi_integer2 
#define mpi_integer4 yogimpi_integer4 
#define mpi_integer8 yogimpi_integer8
#define mpi_real4 yogimpi_real4
#define mpi_real8 yogimpi_real8

! reserved communicators (c and fortran)
#define mpi_comm_world yogimpi_comm_world
#define mpi_comm_self yogimpi_comm_self

! results of communicator and group comparisons
#define mpi_ident yogimpi_ident
#define mpi_congruent yogimpi_congruent
#define mpi_similar yogimpi_similar
#define mpi_unequal yogimpi_unequal

! environmental inquiry keys (c and fortran)
#define mpi_tag_ub yogimpi_tag_ub
#define mpi_io yogimpi_io
#define mpi_host yogimpi_host
#define mpi_wtime_is_global yogimpi_wtime_is_global

! collective operations (c and fortran)
#define mpi_max yogimpi_max
#define mpi_min yogimpi_min
#define mpi_sum yogimpi_sum
#define mpi_prod yogimpi_prod
#define mpi_maxloc yogimpi_maxloc
#define mpi_minloc yogimpi_minloc
#define mpi_band yogimpi_band
#define mpi_bor yogimpi_bor
#define mpi_bxor yogimpi_bxor
#define mpi_land yogimpi_land
#define mpi_lor yogimpi_lor
#define mpi_lxor yogimpi_lxor

! null handles

#define mpi_group_null yogimpi_group_null
#define mpi_comm_null yogimpi_comm_null
#define mpi_request_null yogimpi_request_null
#define mpi_datatype_null yogimpi_datatype_null
#define mpi_info_null yogimpi_info_null
#define mpi_file_null yogimpi_file_null

#define mpi_group_empty yogimpi_group_empty

#define mpi_mode_rdonly yogimpi_mode_rdonly
#define mpi_mode_rdwr yogimpi_mode_rdwr
#define mpi_mode_wronly yogimpi_mode_wronly
#define mpi_mode_create yogimpi_mode_create
#define mpi_mode_excl yogimpi_mode_excl
#define mpi_mode_delete_on_close yogimpi_mode_delete_on_close
#define mpi_mode_unique_open yogimpi_mode_unique_open
#define mpi_mode_sequential yogimpi_mode_sequential
#define mpi_mode_append yogimpi_mode_append

#define mpi_offset_kind yogimpi_offset_kind

#define mpi_status_ignore yogimpi_status_ignore
#define mpi_statuses_ignore yogimpi_statuses_ignore
#define mpi_wtime yogimpi_wtime

#define mpi_send yogimpi_send
#define mpi_recv yogimpi_recv
#define mpi_get_count yogimpi_get_count
#define mpi_ssend yogimpi_ssend
#define mpi_isend yogimpi_isend
#define mpi_issend yogimpi_issend
#define mpi_irecv yogimpi_irecv
#define mpi_wait yogimpi_wait
#define mpi_request_free yogimpi_request_free
#define mpi_waitsome yogimpi_waitsome
#define mpi_waitany yogimpi_waitany
#define mpi_waitall yogimpi_waitall
#define mpi_send_init yogimpi_send_init
#define mpi_type_size yogimpi_type_size
#define mpi_type_contiguous yogimpi_type_contiguous
#define mpi_type_vector yogimpi_type_vector
#define mpi_type_indexed yogimpi_type_indexed
#define mpi_type_commit yogimpi_type_commit
#define mpi_type_free yogimpi_type_free
#define mpi_barrier yogimpi_barrier
#define mpi_bcast yogimpi_bcast
#define mpi_gather yogimpi_gather
#define mpi_gatherv yogimpi_gatherv
#define mpi_scatter yogimpi_scatter
#define mpi_scatterv yogimpi_scatterv
#define mpi_allgather yogimpi_allgather
#define mpi_allgatherv yogimpi_allgatherv
#define mpi_reduce yogimpi_reduce
#define mpi_allreduce yogimpi_allreduce
#define mpi_comm_create yogimpi_comm_create
#define mpi_comm_group yogimpi_comm_group
#define mpi_comm_size yogimpi_comm_size
#define mpi_comm_rank yogimpi_comm_rank
#define mpi_comm_dup yogimpi_comm_dup
#define mpi_comm_split yogimpi_comm_split
#define mpi_comm_free yogimpi_comm_free
#define mpi_group_free yogimpi_group_free
#define mpi_group_incl yogimpi_group_incl
#define mpi_group_rank yogimpi_group_rank
#define mpi_group_translate_ranks yogimpi_group_translate_ranks
#define mpi_get_processor_name yogimpi_get_processor_name
#define mpi_wtime yogimpi_wtime
#define mpi_init yogimpi_init
#define mpi_finalize yogimpi_finalize
#define mpi_type_create_indexed_block yogimpi_type_create_indexed_block
#define mpi_comm_f2c yogimpi_comm_f2c
#define mpi_recv_init yogimpi_recv_init
#define mpi_scan yogimpi_scan
#define mpi_startall yogimpi_startall
#define mpi_alltoall yogimpi_alltoall
#define mpi_alltoallv yogimpi_alltoallv
#define mpi_file_close yogimpi_file_close
#define mpi_file_get_info yogimpi_file_get_info
#define mpi_file_open yogimpi_file_open
#define mpi_file_set_view yogimpi_file_set_view
#define mpi_file_write_all yogimpi_file_write_all
#define mpi_file_write_at yogimpi_file_write_at
#define mpi_info_create yogimpi_info_create
#define mpi_info_set yogimpi_info_set
#define mpi_test yogimpi_test
#define mpi_probe yogimpi_probe
#define mpi_iprobe yogimpi_iprobe
#define mpi_abort yogimpi_abort
#define mpi_attr_get yogimpi_attr_get
#define mpi_finalized yogimpi_finalized
#define mpi_initialized yogimpi_initialized

! And finally handle the case of using C-like case for MPI
		
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
#define MPI_Info_create YogiMPI_Info_create
#define MPI_Info_set YogiMPI_Info_set
#define MPI_Test YogiMPI_Test
#define MPI_Probe YogiMPI_Probe
#define MPI_Iprobe YogiMPI_Iprobe
#define MPI_Abort YogiMPI_Abort
#define MPI_Attr_get YogiMPI_Attr_get
#define MPI_Finalized YogiMPI_Finalized
#define MPI_Initialized YogiMPI_Initialized
