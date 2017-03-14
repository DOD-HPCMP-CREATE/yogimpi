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

#include <yogimpi.h>
#include <mpi.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> /* for hostname */

/* Total number of non-volatile datatypes.  Accounts for Fortran ones, too. */
static const int YogiMPI_DATATYPE_VOLATILE_OFFSET = 38;

/* Total number of datatypes, volatile and non-volatile. 
 */
static int num_datatypes = 100; 

/* Now have a pointer to a pool of MPI datatypes, handing references to
 * users without exposing the opaque MPI handle.
 */
static MPI_Datatype *datatype_pool = NULL;

/* Total number of times datatype pool was resized. Initially zero. */
static int num_realloc_datatypes = 0;

/* Allocates storage and indexes a new volatile MPI datatype in the YogiMPI 
 * pool.  
 * @param mpi_datatype An actual MPI_Datatype to be archived and indexed.
 * @return YogiMPI_Datatype handle to the actual MPI_Datatype.
 */
static YogiMPI_Datatype add_new_datatype(MPI_Datatype mpi_datatype)
{

  /* Find the next available slot to store a new MPI_Datatype.
   * If there aren't enough slots, increase the number of slots. */
  YogiMPI_Datatype new_slot = YogiMPI_DATATYPE_VOLATILE_OFFSET;
  for( ; datatype_pool[new_slot] != MPI_DATATYPE_NULL && 
         new_slot < num_datatypes; ++new_slot );

  if (new_slot == num_datatypes) {
	/* No space left, so double the array size. */
    int new_num_datatypes = num_datatypes * 2;
    ++num_realloc_datatypes;
    int new_pool_size = new_num_datatypes * sizeof(MPI_Datatype);
    MPI_Datatype *new_datatypes = (MPI_Datatype *)realloc(datatype_pool,
    		                                             new_pool_size);

    /* Fill in all the newly-formed slots with NULL values. */
    int i;
    for(i = num_datatypes; i < new_num_datatypes; ++i) {
    	new_datatypes[i] = MPI_DATATYPE_NULL;
    }

    /* Reassign old pool pointer and counter to larger space and counter. */
    datatype_pool = new_datatypes;
    num_datatypes = new_num_datatypes;
  }
  
  datatype_pool[new_slot] = mpi_datatype;
  return new_slot;
  
}

/* Holds rank of this MPI process */
int my_rank = -1;

/* Array maps YogiMPI error constants to actual MPI constants */
static int mpi_error_codes[38];

static int num_groups = 100; 
/* Pointer to pool of MPI_Group objects */
static MPI_Group *group_pool = NULL;
/* Number of times group pool is reallocated for expansion. */
static int num_realloc_groups = 0; 
/* from this offset onward up to num_groups are volatile in group_pool */
static const YogiMPI_Group YogiMPI_GROUP_VOLATILE_OFFSET = 2; 

/* Pointer to pool of MPI_Comm objects */
static MPI_Comm *comm_pool = NULL;
static int num_comms = 100; 
/* Number of times comm pool is reallocated for expansion. */
static int num_realloc_comms = 0;
/* From this offset onward up to num_comms the communicators in comm_pool are
   volatile */
static const YogiMPI_Comm YogiMPI_COMM_VOLATILE_OFFSET = 3;

static int num_requests = 100;
/* Pointer to pool of MPI_Request objects */
static MPI_Request *request_pool = NULL;
/* Number of times request pool is reallocated for expansion. */
static int num_realloc_requests = 0;

static int num_ops = 20;
/* Pointer to pool of MPI_Op objects */
static MPI_Op *op_pool = NULL;
/* From this offset onward up to num_ops the Op objects in op_pool are
   volatile */
static const YogiMPI_Op YogiMPI_OP_VOLATILE_OFFSET = 13;

/* Do the same thing for MPI_Info and MPI_File pools. */
static int num_files = 100;
static int num_infos = 100;
/* Pointer to pool of MPI_File objects */
static MPI_File *file_pool = NULL;
/* Pointer to pool of MPI_Info objects */
static MPI_Info *info_pool = NULL;
/* From this offset onward up to num_files the File objects in file_pool are
   volatile */
static const YogiMPI_File YogiMPI_FILE_VOLATILE_OFFSET = 1;
/* From this offset onward up to num_infos the Info objects in info_pool are
   volatile */
static const YogiMPI_Info YogiMPI_INFO_VOLATILE_OFFSET = 1;

/* Internal definitions for MPI_Errhandler */
static int num_errhandlers = 20;
/* Pointer to pool of MPI_Errhandler objects */
static MPI_Errhandler *errhandler_pool = NULL;
/* From this offset onward up to num_errhandlers MPI_Errhandlers are
   volatile */
static const YogiMPI_Errhandler YogiMPI_ERRHANDLER_VOLATILE_OFFSET = 3;

/*
 * conversion functions YogiMPI <-> MPI
 */

/* Converts an MPI-error into a YogiMPI-error */
int error_to_yogi(int mpi_error) 
{ 
  int current = 0;
  while(mpi_error_codes[current] != mpi_error) {
    if (current > YogiMPI_ERR_LASTCODE) {
      return YogiMPI_ERR_INTERN;
    }
    ++current;
  }
  return current;
}

/* Convert a YogiMPI-error to MPI-error */
int yogi_error_to_mpi(int yogi_error) {
  return mpi_error_codes[yogi_error];
}

/* Converts a YogiMPI_Datatype to MPI_Datatype */
MPI_Datatype datatype_to_mpi(YogiMPI_Datatype in)
{ 
  return datatype_pool[in]; 
}

/* Converts a YogiMPI_Group to MPI_Group */
MPI_Group group_to_mpi(YogiMPI_Group group)
{
  return group_pool[group];
}

/* Converts a YogiMPI_Comm to MPI_Comm */
MPI_Comm comm_to_mpi(YogiMPI_Comm comm)
{
  return comm_pool[comm];
}

/* Converts a YogiMPI_Comm array to an array of MPI_Comm objects */ 
MPI_Comm * comm_array_to_mpi(YogiMPI_Comm array_of_comms[], int num) {
    MPI_Comm* mpi_comms = (MPI_Comm*)malloc(num*sizeof(MPI_Comm));
    int i;
    for(i = 0; i < num; ++i) {
        mpi_comms[i] = comm_to_mpi(array_of_comms[i]);
    }
    return mpi_comms;
}

/* Frees an array of MPI Comm objects */
void free_comm_array(MPI_Comm array_of_comms[]) {
    free(array_of_comms);
}

/* Converts a YogiMPI_Datatype array to an array of MPI_Datatype objects */
MPI_Datatype * datatype_array_to_mpi(YogiMPI_Datatype array_of_types[],
                                     int num) {
    MPI_Datatype* mpi_types = (MPI_Datatype*)malloc(num*sizeof(MPI_Datatype));
    int i;
    for(i = 0; i < num; ++i) {
        mpi_types[i] = datatype_to_mpi(array_of_types[i]);
    }
    return mpi_types;
}

/* Frees an array of MPI_Datatype objects */
void free_datatype_array(MPI_Datatype *array_of_types) {
	if (array_of_types != NULL) {
	    free(array_of_types);
	}
}

/* Converts a YogiMPI_Request to MPI_Request pointer */
MPI_Request* request_to_mpi(YogiMPI_Request request) 
{
    return &request_pool[request];
}

/* Converts a YogiMPI_Op to MPI_Op */
MPI_Op op_to_mpi(YogiMPI_Op op)
{ 
    return op_pool[op]; 
}

/* Converts a YogiMPI_File to MPI_File */
MPI_File file_to_mpi(YogiMPI_File file)
{ 
    return file_pool[file]; 
}

/* Converts a YogiMPI_Info to MPI_Info */
MPI_Info info_to_mpi(YogiMPI_Info info)
{ 
    return info_pool[info]; 
}

/* Converts a YogiMPI_Errhandler to MPI_Errhandler */
MPI_Errhandler errhandler_to_mpi(YogiMPI_Errhandler errhandler) {
	return errhandler_pool[errhandler];
}

/* Convert MPI comparison constants to YogiMPI comparison constants */
int comparison_to_yogi(int mpi_comp)
{
  if (mpi_comp == MPI_IDENT) return YogiMPI_IDENT;
  if (mpi_comp == MPI_CONGRUENT) return YogiMPI_CONGRUENT;
  if (mpi_comp == MPI_SIMILAR) return YogiMPI_SIMILAR;
  if (mpi_comp == MPI_UNEQUAL) return YogiMPI_UNEQUAL;
  return YogiMPI_UNEQUAL;
}

/* Register pre-indexed group */
static void register_preindexed_group(YogiMPI_Group group, MPI_Group mpi_group) 
{
  assert(group >= 0 && group < YogiMPI_GROUP_VOLATILE_OFFSET);
  group_pool[group] = mpi_group;
}

/* Allocate a new YogiMPI_Group that corresponds to the given MPI_Group
 *
 * 1) Find an available 'index' for a new YogiMPI_Group
 * 2) Save the MPI_Group at the index 
 * 3) Set the ref-count to 1
 * 
 * This routine is only used for volatile groups because the others have
 * a predefined index. Thus mpi_group must thus also be a volatile group.
 */
static YogiMPI_Group add_new_group(MPI_Group mpi_group)
{

    /* find new slot */
    YogiMPI_Group new_slot = YogiMPI_GROUP_VOLATILE_OFFSET;
    for(; group_pool[new_slot] != MPI_GROUP_NULL && 
          new_slot < num_groups; ++new_slot);

    /* if not sufficient slots, increase number of slots */
    if (new_slot == num_groups) {
        int new_num_groups = num_groups * 2;
        int i;

        ++num_realloc_groups; /* update stats */

        /* realloc group_pool */
        MPI_Group* new_groups = (MPI_Group*)malloc(new_num_groups*sizeof(MPI_Group));
        memcpy(new_groups,group_pool,num_groups*sizeof(MPI_Group)) ;
        free(group_pool);

        for(i = num_groups; i < new_num_groups; ++i) {
            new_groups[i] = MPI_GROUP_NULL;
        }

        group_pool = new_groups;
        num_groups = new_num_groups;
    }

    assert(new_slot < num_groups);
    assert(group_pool[new_slot] == MPI_GROUP_NULL);
    group_pool[new_slot] = mpi_group;

    return new_slot;
}

static void register_preindexed_comm(YogiMPI_Comm comm, MPI_Comm mpi_comm) {
    assert(comm >= 0 && comm < num_comms);
    comm_pool[comm] = mpi_comm;
}

static YogiMPI_Comm add_new_comm(MPI_Comm mpi_comm) {

    /* find new slot */
    YogiMPI_Comm new_slot = YogiMPI_COMM_VOLATILE_OFFSET;
    for(; comm_pool[new_slot] != MPI_COMM_NULL &&
        new_slot < num_comms; ++new_slot);

    /* if not sufficient slots, increase number of slots */
    if (new_slot == num_comms) {
        int new_num_comms = num_comms * 2;
        int i;

        ++num_realloc_comms; /* update stats */

        /* realloc comms_, comm_groups_ and is_inter_comms_ */
        MPI_Comm* new_comms = (MPI_Comm*)malloc(new_num_comms*sizeof(MPI_Comm));
        memcpy(new_comms,comm_pool,num_comms*sizeof(MPI_Comm));
        free(comm_pool);

        for(i = num_comms; i < new_num_comms; ++i) new_comms[i] = MPI_COMM_NULL;

        comm_pool = new_comms;
        num_comms = new_num_comms;
    }

    assert(comm_pool[new_slot] == MPI_COMM_NULL);
    comm_pool[new_slot] = mpi_comm;

    return new_slot;
}

static YogiMPI_Request add_new_request(MPI_Request mpi_request)
{
    /* Find a slot, start from 1 because 0 == YogiMPI_REQUEST_NULL */
    YogiMPI_Request request = 1; 
    for(; request < num_requests && request_pool[request] != MPI_REQUEST_NULL;
    	++request);

    /* if not sufficient slots, increase number of slots */
    if (request == num_requests) {
        int new_num_requests = num_requests * 2;
        ++num_realloc_requests;

        /* realloc */
        MPI_Request* new_requests = (MPI_Request*)malloc(new_num_requests*sizeof(MPI_Request));
        memcpy(new_requests,request_pool,num_requests*sizeof(MPI_Request));
        free(request_pool);

        int i;
        for(i = num_requests; i < new_num_requests; ++i) {
        	new_requests[i] = MPI_REQUEST_NULL;
        }

        request_pool = new_requests;
        num_requests = new_num_requests;
    }

    assert(request < num_requests);
    assert(request_pool[request] == MPI_REQUEST_NULL);
    request_pool[request] = mpi_request;

    return request;
}

static YogiMPI_File add_new_file(MPI_File mpi_file)
{
    /* Find a slot, start from 1 because 0 == YogiMPI_FILE_NULL */
    YogiMPI_File file = 1; 
    for(; file < num_files && file_pool[file] != MPI_FILE_NULL; ++file);

    /* if not sufficient slots, increase number of slots */
    if (file == num_files) {
        int new_num_files = num_files * 2;

        /* realloc */
        MPI_File* new_files = (MPI_File*)malloc(new_num_files*sizeof(MPI_File));
        memcpy(new_files,file_pool,num_files*sizeof(MPI_File));
        free(file_pool);

        int i;
        for(i = num_files; i < new_num_files; ++i) {
        	new_files[i] = MPI_FILE_NULL;
        }

        file_pool = new_files;
        num_files = new_num_files;
    }

    assert(file < num_files);
    assert(file_pool[file] == MPI_FILE_NULL);
    file_pool[file] = mpi_file;

    return file;
}

static YogiMPI_Info add_new_info(MPI_Info mpi_info)
{
    /* Find a slot, start from 1 because 0 == YogiMPI_INFO_NULL */
    YogiMPI_Info info = 1; 
    for(; info < num_infos && info_pool[info] != MPI_INFO_NULL; ++info);

    /* if not sufficient slots, increase number of slots */
    if (info == num_infos) {
        int new_num_infos = num_infos * 2;

        /* realloc */
        MPI_Info* new_infos = (MPI_Info*)malloc(new_num_infos*sizeof(MPI_Info));
        memcpy(new_infos,info_pool,num_infos*sizeof(MPI_Info));
        free(info_pool);

        int i;
        for(i = num_infos; i < new_num_infos; ++i) {
        	new_infos[i] = MPI_INFO_NULL;
        }

        info_pool = new_infos;
        num_infos = new_num_infos;
    }

    assert(info < num_infos);
    assert(info_pool[info] == MPI_INFO_NULL);
    info_pool[info] = mpi_info;

    return info;
}

static YogiMPI_Errhandler add_new_errhandler(MPI_Errhandler mpi_errhandler)
{
    int i; 
    for(i = YogiMPI_ERRHANDLER_VOLATILE_OFFSET; i < num_errhandlers &&
        errhandler_pool[i] != MPI_ERRHANDLER_NULL; ++i);

    /* if not sufficient slots, increase number of slots */
    if (i == num_errhandlers) {
        int new_num_errhandlers = num_errhandlers * 2;

        /* realloc */
        MPI_Errhandler* new_errhandlers = (MPI_Errhandler*)malloc(new_num_errhandlers
        		                          *sizeof(MPI_Errhandler));
        memcpy(new_errhandlers,errhandler_pool,num_errhandlers*sizeof(MPI_Errhandler));
        free(errhandler_pool);

        int j;
        for(j = num_errhandlers; j < new_num_errhandlers; ++j) {
        	new_errhandlers[j] = MPI_ERRHANDLER_NULL;
        }

        errhandler_pool = new_errhandlers;
        num_errhandlers = new_num_errhandlers;
    }

    assert(i < num_errhandlers);
    assert(errhandler_pool[i] == MPI_ERRHANDLER_NULL);
    errhandler_pool[i] = mpi_errhandler;

    return i;
}

static YogiMPI_Op add_new_op(MPI_Op mpi_op)
{
    int i; 
    for(i = YogiMPI_OP_VOLATILE_OFFSET; i < num_ops &&
        op_pool[i] != MPI_OP_NULL; ++i);

    /* if not sufficient slots, increase number of slots */
    if (i == num_ops) {
        int new_num_ops = num_ops * 2;

        /* realloc */
        MPI_Op* new_ops = (MPI_Op*)malloc(new_num_ops*sizeof(MPI_Op));
        memcpy(new_ops, op_pool, num_ops*sizeof(MPI_Op));
        free(op_pool);

        int j;
        for(j = num_ops; j < new_num_ops; ++j) {
        	new_ops[j] = MPI_OP_NULL;
        }

        op_pool = new_ops;
        num_ops = new_num_ops;
    }

    assert(i < num_ops);
    assert(op_pool[i] == MPI_OP_NULL);
    op_pool[i] = mpi_op;

    return i;
}

/* Copy an MPI_Status pointer into a YogiMPI_Status object.
 * @arg source The MPI_Status memory address from which to copy.
 * @arg dest The YogiMPI_Status memory address into which copy is placed.
 */
static void mpi_status_to_yogi(MPI_Status *source, YogiMPI_Status *dest) {
    dest->MPI_TAG = source->MPI_TAG;
    dest->MPI_SOURCE = source->MPI_SOURCE;
    dest->MPI_ERROR = source->MPI_ERROR;
    /* If this isn't the same address, force a memcpy */
    if ((void *)dest->realStatus != (void *)source) {
        memcpy((void *)dest->realStatus, (void *)source, sizeof(MPI_Status));
    }
}

/* Retrieve the real MPI_Status pointer from a YogiMPI_Status object */
static MPI_Status * yogi_status_to_mpi(YogiMPI_Status *source)
{
    /* This will grab the number of bytes needed.  We don't care about
     * structure padding since this area is never directly accessed by us.
     * It is ensured to be larger than we need.
    */
    return (MPI_Status *)&source->realStatus[0];
}

static MPI_Offset offset_to_mpi(YogiMPI_Offset in) {
    return (MPI_Offset)	in;
}

static MPI_Aint aint_to_mpi(YogiMPI_Aint in) {
	return (MPI_Aint) in;
}

static YogiMPI_Aint aint_to_yogi(MPI_Aint in) {
	return (YogiMPI_Aint) in;
}

static YogiMPI_Offset offset_to_yogi(MPI_Offset in) {
	return (YogiMPI_Offset) in;
}

static int yogi_amode_to_mpi(int amode) {
    switch(amode) {
    case YogiMPI_MODE_RDONLY: 
	    return MPI_MODE_RDONLY;
	    break;
    case YogiMPI_MODE_RDWR:
	    return MPI_MODE_RDWR;
	    break;
    case YogiMPI_MODE_WRONLY:
	    return MPI_MODE_WRONLY;
      	break;
    case YogiMPI_MODE_CREATE: 
	    return MPI_MODE_CREATE;
	    break;
    case YogiMPI_MODE_EXCL: 
	    return MPI_MODE_EXCL;
        break;
    case YogiMPI_MODE_DELETE_ON_CLOSE: 
	    return MPI_MODE_DELETE_ON_CLOSE;
	    break;
    case YogiMPI_MODE_UNIQUE_OPEN: 
	    return MPI_MODE_UNIQUE_OPEN;
	    break;
    case YogiMPI_MODE_SEQUENTIAL: 
	    return MPI_MODE_SEQUENTIAL;
	    break;
    case YogiMPI_MODE_APPEND:
	    return MPI_MODE_APPEND;
	    break;
    default:
    	return amode;
    }

	
}

/* Convert root constants in the case of an intercommunicator. */
static int root_to_mpi(int root) {
    if (root == YogiMPI_ROOT) return MPI_ROOT;
    if (root == YogiMPI_PROC_NULL) return MPI_PROC_NULL;
    return root;
}

/* Checks if an MPI_Datatype is present in existing datatype pool.
 * Returns index of location of datatype, or -1 if not found.
 */
static int check_datatype_presence(MPI_Datatype type_input) {
    int current = 0;
    while(current < num_datatypes) {
        if (datatype_pool[current] == type_input) {
            return current;
        }
        ++current;
    }
    return -1;	
}

/* Checks if an MPI_Comm is present in existing comm pool.
 * Returns index of location of comm, or -1 if not found.
 */
static int check_comm_presence(MPI_Comm comm_input) {
    int current = 0;
    while(current < num_comms) {
        if (comm_pool[current] == comm_input) {
	    return current;
	}
	++current;//
    }
    return -1;	
}


/* Convert an MPI_Datatype array to a YogiMPI_Datatype array. This adds new
 * handles to the datatype pool. */
static void datatype_array_to_yogi(MPI_Datatype *inputArray,
                                   YogiMPI_Datatype *outputArray,
                                   int count) {
    int i;
    for (i = 0; i < count; i++) {
    	/* See if this datatype is already registered in the pool.  If so,
    	 * just give the existing index.  If not, add it to the pool.
    	 * This is needed to make MPI_Type_get_contents work properly.
    	 */
        int location = check_datatype_presence(inputArray[i]);
        if (location >= 0) {
            outputArray[i] = location;
        }
        else {
            outputArray[i] = add_new_datatype(inputArray[i]);
        }
    }
} 

static MPI_Aint * aint_array_to_mpi(YogiMPI_Aint in[], int count) {
    int i;
    MPI_Aint * conv_aint = (MPI_Aint *)malloc(count*sizeof(MPI_Aint));
    for (i = 0; i < count; i++) {
        conv_aint[i] = aint_to_yogi(in[i]);
    }
    return conv_aint;	
}

/* Convert an MPI_Aint array to a YogiMPI_Aint array. */
static void aint_array_to_yogi(MPI_Aint *inputArray, YogiMPI_Aint *outputArray,
                               int count) {
    int i;
    for (i = 0; i < count; i++) {
        outputArray[i] = aint_to_mpi(inputArray[i]);
    }
} 

static MPI_Offset * offset_array_to_mpi(YogiMPI_Offset in[], int count) {
	int i;
	MPI_Offset * conv_offset = (MPI_Offset *)malloc(count*sizeof(MPI_Offset));
	for (i = 0; i < count; i++) {
		conv_offset[i] = offset_to_mpi(in[i]);
	}
	return conv_offset;
}

static void free_aint_array(MPI_Aint * in) {
    if (in != NULL) {
        free(in);
    }
}

static void free_offset_array(MPI_Offset * in) {
    if (in != NULL) {
        free(in);
    }
}

YogiMPI_Comm fortran_comm_to_yogi(int fortran_comm) {
    MPI_Comm c_comm = MPI_Comm_f2c(fortran_comm);
    return add_new_comm(c_comm); 
}

YogiMPI_Group fortran_group_to_yogi(int fortran_group) {
    MPI_Group c_group = MPI_Group_f2c(fortran_group);
    return add_new_group(c_group);
}

YogiMPI_Comm Yogi_ResolveComm(void *commObject) {
    MPI_Comm *aComm = (MPI_Comm*)commObject;
    return check_comm_presence(*aComm);
}

YogiMPI_Comm Yogi_ResolveComm_f2c(int *fCommObject) {
    MPI_Comm cver = MPI_Comm_f2c(*fCommObject); 
    return Yogi_ResolveComm(&cver);
}

YogiMPI_Datatype Yogi_ResolveDatatype(void *datatypeObject) {
    MPI_Datatype *aType = (MPI_Datatype*)datatypeObject;
    return check_datatype_presence(*aType);
}

int Yogi_ResolveErrorcode(int errorcode) {
    return error_to_yogi(errorcode);
}

static void bind_mpi_err_constants() {

    mpi_error_codes[YogiMPI_SUCCESS]         = MPI_SUCCESS;
    mpi_error_codes[YogiMPI_ERR_BUFFER]      = MPI_ERR_BUFFER;
    mpi_error_codes[YogiMPI_ERR_COUNT]       = MPI_ERR_COUNT;
    mpi_error_codes[YogiMPI_ERR_TYPE]        = MPI_ERR_TYPE;
    mpi_error_codes[YogiMPI_ERR_TAG]         = MPI_ERR_TAG;
    mpi_error_codes[YogiMPI_ERR_COMM]        = MPI_ERR_COMM;
    mpi_error_codes[YogiMPI_ERR_RANK]        = MPI_ERR_RANK;
    mpi_error_codes[YogiMPI_ERR_REQUEST]     = MPI_ERR_REQUEST;
    mpi_error_codes[YogiMPI_ERR_ROOT]        = MPI_ERR_ROOT;
    mpi_error_codes[YogiMPI_ERR_GROUP]       = MPI_ERR_GROUP;
    mpi_error_codes[YogiMPI_ERR_OP]          = MPI_ERR_OP;
    mpi_error_codes[YogiMPI_ERR_TOPOLOGY]    = MPI_ERR_TOPOLOGY;
    mpi_error_codes[YogiMPI_ERR_DIMS]        = MPI_ERR_DIMS;
    mpi_error_codes[YogiMPI_ERR_ARG]         = MPI_ERR_ARG;
    mpi_error_codes[YogiMPI_ERR_UNKNOWN]     = MPI_ERR_UNKNOWN;
    mpi_error_codes[YogiMPI_ERR_TRUNCATE]    = MPI_ERR_TRUNCATE;
    mpi_error_codes[YogiMPI_ERR_OTHER]       = MPI_ERR_OTHER;
    mpi_error_codes[YogiMPI_ERR_INTERN]      = MPI_ERR_INTERN;
    mpi_error_codes[YogiMPI_ERR_PENDING]     = MPI_ERR_PENDING;
    mpi_error_codes[YogiMPI_ERR_IN_STATUS]   = MPI_ERR_IN_STATUS;
    mpi_error_codes[YogiMPI_ERR_FILE]        = MPI_ERR_FILE;
    mpi_error_codes[YogiMPI_ERR_NOT_SAME]    = MPI_ERR_NOT_SAME;
    mpi_error_codes[YogiMPI_ERR_AMODE]       = MPI_ERR_AMODE;
    mpi_error_codes[YogiMPI_ERR_UNSUPPORTED_DATAREP] 
                                              = MPI_ERR_UNSUPPORTED_DATAREP;
    mpi_error_codes[YogiMPI_ERR_UNSUPPORTED_OPERATION] 
                                              = MPI_ERR_UNSUPPORTED_OPERATION;
    mpi_error_codes[YogiMPI_ERR_NO_SUCH_FILE] = MPI_ERR_NO_SUCH_FILE;
    mpi_error_codes[YogiMPI_ERR_FILE_EXISTS]  = MPI_ERR_FILE_EXISTS;
    mpi_error_codes[YogiMPI_ERR_BAD_FILE]     = MPI_ERR_BAD_FILE;
    mpi_error_codes[YogiMPI_ERR_ACCESS]       = MPI_ERR_ACCESS; 
    mpi_error_codes[YogiMPI_ERR_NO_SPACE]     = MPI_ERR_NO_SPACE; 
    mpi_error_codes[YogiMPI_ERR_QUOTA]        = MPI_ERR_QUOTA; 
    mpi_error_codes[YogiMPI_ERR_READ_ONLY]    = MPI_ERR_READ_ONLY; 
    mpi_error_codes[YogiMPI_ERR_FILE_IN_USE]  = MPI_ERR_FILE_IN_USE; 
    mpi_error_codes[YogiMPI_ERR_DUP_DATAREP]  = MPI_ERR_DUP_DATAREP; 
    mpi_error_codes[YogiMPI_ERR_CONVERSION]   = MPI_ERR_CONVERSION; 
    mpi_error_codes[YogiMPI_ERR_IO]           = MPI_ERR_IO;
    mpi_error_codes[YogiMPI_ERR_LASTCODE]     = MPI_ERR_LASTCODE;

}

static void initialize_errhandler_pool() {
    int i;
    assert(!errhandler_pool);
  
    errhandler_pool = (MPI_Errhandler *)malloc(sizeof(MPI_Errhandler)*num_errhandlers);
    for(i = 0; i < num_errhandlers; ++i) {
    	errhandler_pool[i] = MPI_ERRHANDLER_NULL;
    }
    
	/* Predefined error handlers */
	errhandler_pool[YogiMPI_ERRHANDLER_NULL]  = MPI_ERRHANDLER_NULL;
	errhandler_pool[YogiMPI_ERRORS_ARE_FATAL] = MPI_ERRORS_ARE_FATAL;
	errhandler_pool[YogiMPI_ERRORS_RETURN]    = MPI_ERRORS_RETURN;
}

static void initialize_datatype_pool() {
    
    int i;
    assert(!datatype_pool);
    assert(num_datatypes > YogiMPI_PACKED);
  
    /* Initial allocation of datatype pool */
    datatype_pool = (MPI_Datatype *)malloc(sizeof(MPI_Datatype)*num_datatypes);
    for(i = 0; i < num_datatypes; ++i) {
    	datatype_pool[i] = MPI_DATATYPE_NULL;
    }
	datatype_pool[YogiMPI_CHAR]              = MPI_CHAR;
	datatype_pool[YogiMPI_SHORT]             = MPI_SHORT;
	datatype_pool[YogiMPI_INT]               = MPI_INT;
	datatype_pool[YogiMPI_LONG]              = MPI_LONG;
	datatype_pool[YogiMPI_UNSIGNED_CHAR]     = MPI_UNSIGNED_CHAR;
	datatype_pool[YogiMPI_UNSIGNED_SHORT]    = MPI_UNSIGNED_SHORT;
	datatype_pool[YogiMPI_UNSIGNED]          = MPI_UNSIGNED;
	datatype_pool[YogiMPI_UNSIGNED_LONG]     = MPI_UNSIGNED_LONG;
	datatype_pool[YogiMPI_FLOAT]             = MPI_FLOAT;
	datatype_pool[YogiMPI_DOUBLE]            = MPI_DOUBLE;
	datatype_pool[YogiMPI_LONG_DOUBLE]       = MPI_LONG_DOUBLE;
	datatype_pool[YogiMPI_BYTE]              = MPI_BYTE;
	datatype_pool[YogiMPI_PACKED]            = MPI_PACKED;
	datatype_pool[YogiMPI_FLOAT_INT]         = MPI_FLOAT_INT;
	datatype_pool[YogiMPI_DOUBLE_INT]        = MPI_DOUBLE_INT;
	datatype_pool[YogiMPI_LONG_INT]          = MPI_LONG_INT;
	datatype_pool[YogiMPI_2INT]              = MPI_2INT;
	datatype_pool[YogiMPI_SHORT_INT]         = MPI_SHORT_INT;
	datatype_pool[YogiMPI_LONG_DOUBLE_INT]   = MPI_LONG_DOUBLE_INT;
	datatype_pool[YogiMPI_LONG_LONG_INT]     = MPI_LONG_LONG_INT;
	datatype_pool[YogiMPI_INT32_T]           = MPI_INT32_T;
	datatype_pool[YogiMPI_INT64_T]           = MPI_INT64_T;

	datatype_pool[YogiMPI_COMPLEX]           = MPI_COMPLEX;
        datatype_pool[YogiMPI_DOUBLE_COMPLEX]    = MPI_DOUBLE_COMPLEX;
	datatype_pool[YogiMPI_LOGICAL]           = MPI_LOGICAL;
	datatype_pool[YogiMPI_2REAL]             = MPI_2REAL;
	datatype_pool[YogiMPI_2DOUBLE_PRECISION] = MPI_2DOUBLE_PRECISION; 
	datatype_pool[YogiMPI_2INTEGER]          = MPI_2INTEGER;
	datatype_pool[YogiMPI_INTEGER1]          = MPI_INTEGER1;
	datatype_pool[YogiMPI_INTEGER2]          = MPI_INTEGER2;
	datatype_pool[YogiMPI_INTEGER4]          = MPI_INTEGER4;
	datatype_pool[YogiMPI_INTEGER8]          = MPI_INTEGER8;
	datatype_pool[YogiMPI_REAL4]             = MPI_REAL4;
	datatype_pool[YogiMPI_REAL8]             = MPI_REAL8;
        datatype_pool[YogiMPI_UNSIGNED_LONG_LONG] = MPI_UNSIGNED_LONG_LONG;
        datatype_pool[YogiMPI_LB]                = MPI_LB;
        datatype_pool[YogiMPI_UB]                = MPI_UB;
        datatype_pool[YogiMPI_SIGNED_CHAR]       = MPI_SIGNED_CHAR;	
}

static void initialize_group_pool() {
	
	int i;
    assert(!group_pool);
    group_pool = (MPI_Group *)malloc(sizeof(MPI_Group)*num_groups);
    for(i = 0; i < num_groups; ++i) group_pool[i] = MPI_GROUP_NULL;

    register_preindexed_group(YogiMPI_GROUP_NULL, MPI_GROUP_NULL);
    register_preindexed_group(YogiMPI_GROUP_EMPTY, MPI_GROUP_EMPTY);

}

static void initialize_comm_pool() {
    int i;
    assert(!comm_pool);
    comm_pool = (MPI_Comm *)malloc(sizeof(MPI_Comm)*num_comms);
    
    register_preindexed_comm(YogiMPI_COMM_NULL, MPI_COMM_NULL);
    register_preindexed_comm(YogiMPI_COMM_WORLD, MPI_COMM_WORLD);
    register_preindexed_comm(YogiMPI_COMM_SELF, MPI_COMM_SELF);	
}

static void initialize_request_pool() {
    int i;
    request_pool = (MPI_Request *)malloc(num_requests*sizeof(MPI_Request));
    for(i = 0; i < num_requests; ++i) request_pool[i] = MPI_REQUEST_NULL;	
}

static void initialize_file_pool() {
    int i;
    file_pool = (MPI_File *)malloc(num_files*sizeof(MPI_File));
    for(i = 0; i < num_files; ++i) file_pool[i] = MPI_FILE_NULL;	
}

static void initialize_info_pool() {
    int i;
    info_pool = (MPI_Info *)malloc(num_infos*sizeof(MPI_Info));
    for(i = 0; i < num_infos; ++i) info_pool[i] = MPI_INFO_NULL;	
}

static void initialize_op_pool() {
    int i;
    assert(num_ops > YogiMPI_OP_VOLATILE_OFFSET);
    op_pool = (MPI_Op *)malloc(num_ops*sizeof(MPI_Op));
    for(i = 0; i < num_ops; ++i ) op_pool[i] = MPI_OP_NULL;
    op_pool[YogiMPI_MAX]    = MPI_MAX;
    op_pool[YogiMPI_MIN]    = MPI_MIN;
    op_pool[YogiMPI_SUM]    = MPI_SUM;
    op_pool[YogiMPI_PROD]   = MPI_PROD;
    op_pool[YogiMPI_MAXLOC] = MPI_MAXLOC;
    op_pool[YogiMPI_MINLOC] = MPI_MINLOC;
    op_pool[YogiMPI_BAND]   = MPI_BAND;
    op_pool[YogiMPI_BOR]    = MPI_BOR;
    op_pool[YogiMPI_BXOR]   = MPI_BXOR;
    op_pool[YogiMPI_LAND]   = MPI_LAND;
    op_pool[YogiMPI_LOR]    = MPI_LOR;
    op_pool[YogiMPI_LXOR]   = MPI_LXOR;	
}

void allocate_yogimpi_storage() {

    assert(YogiMPI_BSEND_OVERHEAD >= MPI_BSEND_OVERHEAD);
    
    /* Initialize the back-end arrays for opaque objects and references */
    bind_mpi_err_constants();
    if (datatype_pool == NULL) initialize_datatype_pool();
    if (errhandler_pool == NULL) initialize_errhandler_pool();
    if (group_pool == NULL) initialize_group_pool();
    if (comm_pool == NULL) initialize_comm_pool();
    if (request_pool == NULL) initialize_request_pool();
    if (op_pool == NULL) initialize_op_pool();
    if (info_pool == NULL) initialize_info_pool();
    if (file_pool == NULL) initialize_file_pool();

}

void deallocate_yogimpi_storage(void) {

    if (request_pool != NULL) {
        free(request_pool);
        request_pool = NULL;
    }
    if (comm_pool != NULL) {
        free(comm_pool);
        comm_pool = NULL;
    }
    if (group_pool != NULL) {
        free(group_pool);
        group_pool = NULL;
    }
    if (info_pool != NULL) {
        free(info_pool);
        info_pool = NULL;
    }
    if (file_pool != NULL) {
        free(file_pool);
        file_pool = NULL;
    }
    if (op_pool != NULL) {
        free(op_pool);
        op_pool = NULL;
    }
    if (datatype_pool != NULL) {
        free(datatype_pool);
        datatype_pool = NULL;
    }
    if (errhandler_pool != NULL) {
        free(errhandler_pool);
    	errhandler_pool = NULL;
    }
}

int YogiMPI_Init(int* argc, char ***argv)
{ 

    allocate_yogimpi_storage();

    int mpi_err = MPI_Init(argc,argv); 

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 
    return error_to_yogi(mpi_err);
}

int YogiMPI_Send(const void* buf, int count, YogiMPI_Datatype datatype,
                 int dest, int tag, YogiMPI_Comm comm) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);

    int mpi_error = MPI_Send(buf, count, mpi_datatype, dest, tag, mpi_comm);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Recv(void* buf, int count, YogiMPI_Datatype datatype, int source,
                 int tag, YogiMPI_Comm comm, YogiMPI_Status *status) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    if (YogiMPI_ANY_SOURCE == source) source = MPI_ANY_SOURCE;
    if (YogiMPI_ANY_TAG == tag) tag = MPI_ANY_TAG;
    int mpi_error;
    if (YogiMPI_STATUS_IGNORE != status) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_error = MPI_Recv(buf, count, mpi_datatype, source, tag, mpi_comm, 
                             mpi_status);
        mpi_status_to_yogi(mpi_status, status); 
    }
    else {
        mpi_error = MPI_Recv(buf, count, mpi_datatype, source, tag, mpi_comm, 
                             MPI_STATUS_IGNORE);
    }

    return error_to_yogi(mpi_error);
}

int YogiMPI_Get_count(YogiMPI_Status *status, YogiMPI_Datatype datatype, 
                      int *count) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Status *mpi_status = yogi_status_to_mpi(status);
    int mpi_err = MPI_Get_count(mpi_status, mpi_datatype, count);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Ssend(const void* buf, int count, YogiMPI_Datatype datatype,
                  int dest, int tag, YogiMPI_Comm comm) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);

    int mpi_error = MPI_Ssend(buf, count, mpi_datatype, dest, tag, mpi_comm);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Isend(const void* buf, int count, YogiMPI_Datatype datatype,
                  int dest, int tag, YogiMPI_Comm comm, 
                  YogiMPI_Request *request) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Request mpi_request;

    int mpi_error = MPI_Isend(buf, count, mpi_datatype, dest, tag, mpi_comm, 
                              &mpi_request);
    *request = add_new_request(mpi_request);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Issend(const void* buf, int count, YogiMPI_Datatype datatype, 
                   int dest, int tag, YogiMPI_Comm comm, 
                   YogiMPI_Request *request) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Request mpi_request;

    int mpi_error = MPI_Issend(buf, count, mpi_datatype, dest, tag, mpi_comm, 
                               &mpi_request);
    *request = add_new_request(mpi_request);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Irecv(void* buf, int count, YogiMPI_Datatype datatype, int source, 
                  int tag, YogiMPI_Comm comm, YogiMPI_Request *request) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    if (YogiMPI_ANY_SOURCE == source) source = MPI_ANY_SOURCE;
    if (YogiMPI_ANY_TAG == tag) tag = MPI_ANY_TAG;

    MPI_Request mpi_request;
    int mpi_error = MPI_Irecv(buf, count, mpi_datatype, source, tag, mpi_comm,
                              &mpi_request);
    *request = add_new_request(mpi_request);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Wait(YogiMPI_Request* request, YogiMPI_Status *status) {
	
    MPI_Request* mpi_request = request_to_mpi(*request);
    int mpi_err;
    if (YogiMPI_STATUS_IGNORE != status) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_err = MPI_Wait(mpi_request, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_err = MPI_Wait(mpi_request, MPI_STATUS_IGNORE);
    }
    if (MPI_REQUEST_NULL == *mpi_request) { 
        assert(MPI_REQUEST_NULL == request_pool[*request]);
        *request = YogiMPI_REQUEST_NULL;
    }

    return error_to_yogi(mpi_err);
}

int YogiMPI_Request_free(YogiMPI_Request *request) {
    MPI_Request* mpi_request = request_to_mpi(*request);
    int mpi_err = MPI_Request_free(mpi_request);
    assert(*mpi_request == MPI_REQUEST_NULL);
    *request = YogiMPI_REQUEST_NULL;
    return error_to_yogi(mpi_err);
}

int YogiMPI_Waitall(int count, YogiMPI_Request *array_of_requests, 
                    YogiMPI_Status *array_of_statuses) {
    int i;
    MPI_Request mpi_requests[count];
    for(i = 0; i < count; ++i) {
    	mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }
    int mpi_err;

    if (YogiMPI_STATUSES_IGNORE != array_of_statuses) {
        MPI_Status mpi_statuses[count];
        for (i = 0; i < count; i++) {
            mpi_statuses[i] = *(yogi_status_to_mpi(&array_of_statuses[i]));
        }
        mpi_err = MPI_Waitall(count, mpi_requests, mpi_statuses);
        for(i = 0; i < count; i++) {
            mpi_status_to_yogi(&mpi_statuses[i], &array_of_statuses[i]);
        }
    }
    else {
        mpi_err = MPI_Waitall(count, mpi_requests, MPI_STATUSES_IGNORE);
    }
 
    /* reset requests */
    for(i = 0; i < count; ++i) {
        if(mpi_requests[i] == MPI_REQUEST_NULL) {
            *request_to_mpi(array_of_requests[i]) = MPI_REQUEST_NULL; 
            array_of_requests[i] = YogiMPI_REQUEST_NULL;
        }
    }

    return error_to_yogi(mpi_err);
}

int YogiMPI_Waitsome(int incount, YogiMPI_Request *array_of_requests, 
                     int *outcount, int *array_of_indices, 
                     YogiMPI_Status *array_of_statuses) {
    int i;
    int mpi_err;
    MPI_Request* mpi_requests = (MPI_Request*)malloc(incount*sizeof(MPI_Request));
    for(i = 0; i < incount; ++i) {
        mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }
    
    if (YogiMPI_STATUSES_IGNORE != array_of_statuses ) {
        MPI_Status* mpi_statuses = (MPI_Status*)malloc(incount*sizeof(MPI_Status));
        for (i = 0; i < incount; i++) {
            mpi_statuses[i] = *(yogi_status_to_mpi(&array_of_statuses[i]));
        }
	    mpi_err = MPI_Waitsome(incount, mpi_requests, outcount, 
	                           array_of_indices, mpi_statuses);
        for(i = 0; i < *outcount; i++) {
            mpi_status_to_yogi(&mpi_statuses[i], &array_of_statuses[i]);
        }
        free(mpi_statuses);
    }
    else {
        mpi_err = MPI_Waitsome(incount, mpi_requests, outcount, 
                               array_of_indices, MPI_STATUSES_IGNORE);
    }
    
    /* reset requests */
    if (*outcount == MPI_UNDEFINED) {
        *outcount = YogiMPI_UNDEFINED;
    }
    else {
        for(i = 0; i < *outcount; ++i) {
            int requestIndex = array_of_indices[i];
            if(mpi_requests[requestIndex] == MPI_REQUEST_NULL) {
                *request_to_mpi(array_of_requests[requestIndex]) = MPI_REQUEST_NULL; 
                array_of_requests[requestIndex] = YogiMPI_REQUEST_NULL;
            }
        }
    }
    free(mpi_requests);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Waitany(int count, YogiMPI_Request *array_of_requests, int *index, 
                    YogiMPI_Status *status) {
    int i;
    int mpi_err;
    MPI_Request* mpi_requests = (MPI_Request*)malloc(count*sizeof(MPI_Request));
    for(i = 0; i < count; ++i) {
        mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }
    
    if (YogiMPI_STATUS_IGNORE != status ) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_err = MPI_Waitany(count, mpi_requests, index, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_err = MPI_Waitany(count, mpi_requests, index, MPI_STATUS_IGNORE);
    }
    
    /* reset requests */
    if (*index == MPI_UNDEFINED) {
        *index = YogiMPI_UNDEFINED;
    }
    else {
        if(mpi_requests[*index] == MPI_REQUEST_NULL) {
            *request_to_mpi(array_of_requests[*index]) = MPI_REQUEST_NULL; 
            array_of_requests[*index] = YogiMPI_REQUEST_NULL;
        }
    }
    free(mpi_requests);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Send_init(const void *buf, int count, YogiMPI_Datatype datatype,
                      int dest, int tag, YogiMPI_Comm comm,
                      YogiMPI_Request* request) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Request mpi_request;

    int mpi_error = MPI_Send_init(buf, count, mpi_datatype, dest, tag, mpi_comm,
                                  &mpi_request);
    *request = add_new_request(mpi_request);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_size(const YogiMPI_Datatype datatype, int * size) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    int mpi_err = MPI_Type_size(mpi_datatype, size);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Type_contiguous(int count, YogiMPI_Datatype oldtype, 
                            YogiMPI_Datatype *newtype) {
    MPI_Datatype mpi_oldtype = datatype_to_mpi(oldtype);
    MPI_Datatype mpi_newtype = MPI_DATATYPE_NULL;

    int mpi_err = MPI_Type_contiguous(count, mpi_oldtype, &mpi_newtype);

    *newtype = add_new_datatype(mpi_newtype);

    return error_to_yogi(mpi_err);
}

int YogiMPI_Type_vector(int count, int blocklength, int stride, 
                        YogiMPI_Datatype oldtype, YogiMPI_Datatype *newtype) {
    MPI_Datatype mpi_oldtype = datatype_to_mpi(oldtype);
    MPI_Datatype mpi_newtype = MPI_DATATYPE_NULL;

    int mpi_err = MPI_Type_vector(count, blocklength, stride, mpi_oldtype, 
                                  &mpi_newtype);
    *newtype = add_new_datatype(mpi_newtype);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Type_indexed(int count, int *array_of_blocklengths, 
                        int *array_of_displacements, YogiMPI_Datatype oldtype,
                        YogiMPI_Datatype *newtype) {
    MPI_Datatype mpi_oldtype = datatype_to_mpi(oldtype);
    MPI_Datatype mpi_newtype = MPI_DATATYPE_NULL;

    int mpi_err = MPI_Type_indexed(count, array_of_blocklengths, 
                                   array_of_displacements, mpi_oldtype, 
                                   &mpi_newtype);
    *newtype = add_new_datatype(mpi_newtype);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Type_commit(YogiMPI_Datatype *datatype) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(*datatype);
    int mpi_err = MPI_Type_commit(&mpi_datatype);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Type_free(YogiMPI_Datatype *datatype) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(*datatype);
    int mpi_err = MPI_Type_free(&mpi_datatype);
    datatype_pool[*datatype] = MPI_DATATYPE_NULL;
    *datatype = YogiMPI_DATATYPE_NULL;
    return error_to_yogi(mpi_err);
}

int YogiMPI_Barrier(YogiMPI_Comm comm) {
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    int mpi_err = MPI_Barrier(mpi_comm);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Bcast(void* buffer, int count, YogiMPI_Datatype datatype, int root,
                  YogiMPI_Comm comm) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    int mpi_err = MPI_Bcast(buffer, count, mpi_datatype, root_to_mpi(root),
                            mpi_comm);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Gather(const void* sendbuf, int sendcount, 
                   YogiMPI_Datatype sendtype, void* recvbuf, int recvcount, 
                   YogiMPI_Datatype recvtype, int root, YogiMPI_Comm comm) {

    int mpi_err = YogiMPI_SUCCESS;
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);

    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Gather(MPI_IN_PLACE, sendcount, mpi_sendtype, recvbuf,
                             recvcount, mpi_recvtype, root_to_mpi(root),
                             mpi_comm);
    } 
    else { 
        mpi_err = MPI_Gather(sendbuf, sendcount, mpi_sendtype, recvbuf,
                             recvcount, mpi_recvtype, root_to_mpi(root),
                             mpi_comm);
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Gatherv(const void* sendbuf, int sendcount, 
                    YogiMPI_Datatype sendtype, 
                    void* recvbuf, const int recvcounts[], 
                    const int displs[], YogiMPI_Datatype recvtype, int root,
                    YogiMPI_Comm comm) {
    int mpi_err = YogiMPI_SUCCESS; 
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);

    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Gatherv(MPI_IN_PLACE, sendcount, mpi_sendtype, recvbuf,
                              recvcounts, displs, mpi_recvtype, 
                              root_to_mpi(root), mpi_comm);
    } 
    else { 
        mpi_err = MPI_Gatherv(sendbuf, sendcount, mpi_sendtype, recvbuf,
                              recvcounts, displs, mpi_recvtype, 
                              root_to_mpi(root), mpi_comm);
    }

    return error_to_yogi(mpi_err);
}

int YogiMPI_Scatter(const void *sendbuf, int sendcount,
                    YogiMPI_Datatype sendtype, void *recvbuf, int recvcount,
                    YogiMPI_Datatype recvtype, int root, YogiMPI_Comm comm) {
    int mpi_err = YogiMPI_SUCCESS;
    MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);

    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Scatter(sendbuf, sendcount, mpi_sendtype, MPI_IN_PLACE,
                              recvcount, mpi_recvtype, root_to_mpi(root),
                              mpi_comm);
    } 
    else { 
        mpi_err = MPI_Scatter(sendbuf, sendcount, mpi_sendtype, recvbuf,
                              recvcount, mpi_recvtype, root_to_mpi(root),
                              mpi_comm);
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Scatterv(const void* sendbuf, const int sendcounts[], 
                     const int displs[], YogiMPI_Datatype sendtype,
                     void* recvbuf, int recvcount, YogiMPI_Datatype recvtype,
                     int root, YogiMPI_Comm comm) {
    int mpi_err = YogiMPI_SUCCESS;
    MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);

    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Scatterv(sendbuf, sendcounts, displs, mpi_sendtype,
                               MPI_IN_PLACE, recvcount, mpi_recvtype, 
                               root_to_mpi(root), mpi_comm);
    } 
    else { 
        mpi_err = MPI_Scatterv(sendbuf, sendcounts, displs, mpi_sendtype,
                               recvbuf, recvcount, mpi_recvtype, 
                               root_to_mpi(root), mpi_comm);
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Allgather(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype, 
                      void* recvbuf, int recvcount, YogiMPI_Datatype recvtype, 
                      YogiMPI_Comm comm) {
    int mpi_err = YogiMPI_SUCCESS;
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Allgather(MPI_IN_PLACE, sendcount, mpi_sendtype, recvbuf,
                                recvcount, mpi_recvtype, mpi_comm);
    } 
    else {
        mpi_err = MPI_Allgather(sendbuf, sendcount, mpi_sendtype, recvbuf, 
                                recvcount, mpi_recvtype, mpi_comm);
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Allgatherv(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype, 
                       void* recvbuf, int* recvcounts, int* displs, 
                       YogiMPI_Datatype recvtype, YogiMPI_Comm comm) {

    int mpi_err = YogiMPI_SUCCESS;
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, recvbuf,
                                 recvcounts, displs, mpi_recvtype, mpi_comm);
    } 
    else {
        MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
        mpi_err = MPI_Allgatherv(sendbuf, sendcount, mpi_sendtype, recvbuf,
                                 recvcounts, displs, mpi_recvtype, mpi_comm);
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Reduce(void* sendbuf, void* recvbuf, int count, 
                   YogiMPI_Datatype datatype, YogiMPI_Op op, int root, 
                   YogiMPI_Comm comm) {
    int mpi_err = YogiMPI_SUCCESS;
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype); 
    MPI_Op mpi_op = op_to_mpi(op);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Reduce(MPI_IN_PLACE, recvbuf, count, mpi_datatype, mpi_op,                             root, mpi_comm);
    } 
    else {
        mpi_err = MPI_Reduce(sendbuf, recvbuf, count, mpi_datatype, mpi_op,
                             root_to_mpi(root), mpi_comm);
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Allreduce(void* sendbuf, void* recvbuf, int count, 
                      YogiMPI_Datatype datatype, YogiMPI_Op op, 
                      YogiMPI_Comm comm) {
    int mpi_err = YogiMPI_SUCCESS;
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Op mpi_op = op_to_mpi(op);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Allreduce(MPI_IN_PLACE, recvbuf, count, mpi_datatype,
                                mpi_op, mpi_comm);
    } 
    else {
        mpi_err = MPI_Allreduce(sendbuf, recvbuf, count, mpi_datatype, mpi_op,
                                mpi_comm);
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_create(YogiMPI_Comm comm, YogiMPI_Group group,
                        YogiMPI_Comm *newcomm) {
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Group mpi_group = group_to_mpi(group);
    MPI_Comm mpi_newcomm;

    int mpi_err = MPI_Comm_create(mpi_comm, mpi_group, &mpi_newcomm);
    if (mpi_newcomm == MPI_COMM_NULL) {
        *newcomm = YogiMPI_COMM_NULL;
    }
    else {
        *newcomm = add_new_comm(mpi_newcomm);
    }

    return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_group(YogiMPI_Comm comm, YogiMPI_Group *group) {
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Group mpi_group = MPI_GROUP_NULL;
    MPI_Comm_group(mpi_comm, &mpi_group);
    *group = add_new_group(mpi_group);
    return YogiMPI_SUCCESS;
}

int YogiMPI_Comm_size(YogiMPI_Comm comm, int* size)
{
  int mpi_err = MPI_Comm_size(comm_to_mpi(comm), size);
  return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_rank(YogiMPI_Comm comm, int* rank) { 
  MPI_Comm mpi_comm = comm_to_mpi(comm);
  int mpi_err = MPI_Comm_rank(mpi_comm, rank); 
  return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_dup(YogiMPI_Comm comm, YogiMPI_Comm* out) {
  MPI_Comm mpi_comm = comm_to_mpi(comm);
  MPI_Comm mpi_out;
  int mpi_err = MPI_Comm_dup(mpi_comm, &mpi_out);

  *out = add_new_comm(mpi_out);

  return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_split(YogiMPI_Comm comm, int color, int key, 
		               YogiMPI_Comm* newcomm) {
    
	MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Comm mpi_newcomm = MPI_COMM_NULL;

    if (YogiMPI_UNDEFINED == color) {
        /* Even though color == UNDEFINED, we must perform the call to 
         * comm_split in all procs because this is a global operation and thus 
         * otherwise we might be waiting infinitely 
         */
        int mpi_err = MPI_Comm_split(mpi_comm, MPI_UNDEFINED, key, 
        		                     &mpi_newcomm);
        assert(MPI_COMM_NULL == mpi_newcomm);
        return error_to_yogi(mpi_err);
    }
    else {
        int mpi_err = MPI_Comm_split(mpi_comm, color, key, &mpi_newcomm);

        *newcomm = add_new_comm(mpi_newcomm);
        return error_to_yogi(mpi_err);
    }
}

int YogiMPI_Comm_free(YogiMPI_Comm *comm) {
    assert(comm_pool[*comm] != MPI_COMM_NULL);

    MPI_Comm* mpi_comm = &comm_pool[*comm];
    MPI_Comm_free(mpi_comm);

    comm_pool[*comm] = MPI_COMM_NULL;

    *comm = YogiMPI_COMM_NULL;

    return MPI_SUCCESS;
}

int YogiMPI_Group_free(YogiMPI_Group *group) {
    if (YogiMPI_GROUP_EMPTY == *group) {
        return YogiMPI_SUCCESS;
    }
    else {
        MPI_Group mpi_group = group_pool[*group];
        int mpi_err = MPI_Group_free(&mpi_group);
        assert(mpi_group == MPI_GROUP_NULL);
        group_pool[*group] = MPI_GROUP_NULL;
        *group = YogiMPI_GROUP_NULL;
        return error_to_yogi(mpi_err);
    }
}

int YogiMPI_Group_incl(YogiMPI_Group group, int n, int *ranks, 
		               YogiMPI_Group *group_out) {
	/* See definition of Group_incl in mpi-1.1 */
    if (0 == n) { 
	    *group_out = YogiMPI_GROUP_EMPTY;
	    return YogiMPI_SUCCESS;
	}
	else {
	    MPI_Group mpi_group = group_to_mpi(group);
	    MPI_Group mpi_group_out;
	    int mpi_err = MPI_Group_incl(mpi_group, n, ranks, &mpi_group_out);
	    *group_out = add_new_group(mpi_group_out);

	    return error_to_yogi(mpi_err);
    }

}

int YogiMPI_Group_translate_ranks(YogiMPI_Group group1, int n, int *ranks1, 
		                          YogiMPI_Group group2, int *ranks2) {
    MPI_Group mpi_group1 = group_to_mpi(group1);
    MPI_Group mpi_group2 = group_to_mpi(group2);
    int mpi_err = MPI_Group_translate_ranks(mpi_group1, n, ranks1, mpi_group2,
    		                                ranks2);

    /* convert MPI_UNDEFINED ranks into YogiMPI_UNDEFINED */
    int i = 0;
    for(i = 0; i < n ; ++i) {
        if (MPI_UNDEFINED == ranks2[i]) ranks2[i] = YogiMPI_UNDEFINED;
	}

	return error_to_yogi(mpi_err);
}

int YogiMPI_Group_rank(YogiMPI_Group group, int *rank) {
    MPI_Group mpi_group = group_to_mpi(group);
    int mpi_err = MPI_Group_rank(mpi_group, rank);
	return error_to_yogi(mpi_err);
}

int YogiMPI_Get_processor_name(char *name, int *resultlen) {
    int mpi_err;
    if (MPI_MAX_PROCESSOR_NAME > YogiMPI_MAX_PROCESSOR_NAME) {
        char *tmp_name = (char*)malloc(MPI_MAX_PROCESSOR_NAME);
        mpi_err = MPI_Get_processor_name(tmp_name,resultlen);
        memcpy(name,tmp_name,YogiMPI_MAX_PROCESSOR_NAME-1);
        name[YogiMPI_MAX_PROCESSOR_NAME-1] = '\0';
        free(tmp_name);
        if (*resultlen > YogiMPI_MAX_PROCESSOR_NAME-1) {
            *resultlen = YogiMPI_MAX_PROCESSOR_NAME-1;
        }
    }
    else {
        mpi_err = MPI_Get_processor_name(name, resultlen);
    }
    
    return error_to_yogi(mpi_err);
}

int YogiMPI_Finalize() { 

    int mpi_err = MPI_Finalize();

    deallocate_yogimpi_storage();

    return error_to_yogi(mpi_err);
}

int YogiMPI_Type_create_indexed_block(int count, int blocklength, 
                                      int array_of_displacements[], 
                                      YogiMPI_Datatype oldtype, 
                                      YogiMPI_Datatype *newtype) {
    MPI_Datatype mpi_oldtype = datatype_to_mpi(oldtype);
    MPI_Datatype mpi_newtype = MPI_DATATYPE_NULL;
    int mpi_err = MPI_Type_create_indexed_block(count, blocklength, 
                                                array_of_displacements,
                                                mpi_oldtype, &mpi_newtype);
    *newtype = add_new_datatype(mpi_newtype);
    return error_to_yogi(mpi_err);
}

double YogiMPI_Wtime() {
	return MPI_Wtime();
}

int YogiMPI_Recv_init(void *buf, int count, YogiMPI_Datatype datatype, 
		              int source, int tag, YogiMPI_Comm comm, 
					  YogiMPI_Request *request) {
	
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Request mpi_request;

    int mpi_error = MPI_Recv_init(buf, count, mpi_datatype, source, tag, 
    		                      mpi_comm, &mpi_request);
    *request = add_new_request(mpi_request);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Scan(const void *sendbuf, void *recvbuf, int count, 
		         YogiMPI_Datatype datatype, YogiMPI_Op op, YogiMPI_Comm comm) {

    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Op mpi_op = op_to_mpi(op);

    int mpi_error = MPI_Scan(sendbuf, recvbuf, count, mpi_datatype, mpi_op,
    		                 mpi_comm);
    
    return error_to_yogi(mpi_error);	
}

int YogiMPI_Startall(int count, YogiMPI_Request *array_of_requests) {
    int i;
    MPI_Request* mpi_requests = (MPI_Request*)malloc(count*sizeof(MPI_Request));
    for(i = 0; i < count; i++) {
    	mpi_requests[i] = *request_to_mpi(array_of_requests[i]);
    }
    
    int mpi_error = MPI_Startall(count, mpi_requests);
			
    free(mpi_requests);
    
    return error_to_yogi(mpi_error);	
}

int YogiMPI_Alltoall(const void *sendbuf, int sendcount, 
		     YogiMPI_Datatype sendtype, void *recvbuf, int recvcount,
		     YogiMPI_Datatype recvtype, YogiMPI_Comm comm) {
    MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);

    int mpi_error = MPI_Alltoall(sendbuf, sendcount, mpi_sendtype, recvbuf,
    		                     recvcount, mpi_recvtype, mpi_comm);
    
    return error_to_yogi(mpi_error);	

}

int YogiMPI_Alltoallv(const void *sendbuf, const int *sendcounts,
                      const int *sdispls, YogiMPI_Datatype sendtype, 
                      void *recvbuf, const int *recvcounts, const int *rdispls,
                      YogiMPI_Datatype recvtype, YogiMPI_Comm comm) {

    MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);

    int mpi_error = MPI_Alltoallv(sendbuf, sendcounts, sdispls, mpi_sendtype, 
    		                  recvbuf, recvcounts, rdispls, mpi_recvtype, 
				  mpi_comm);
    
    return error_to_yogi(mpi_error);	

}

int YogiMPI_File_open(YogiMPI_Comm comm, char *filename, int amode, 
		              YogiMPI_Info info, YogiMPI_File *fh) {

    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Info mpi_info = info_to_mpi(info);
    MPI_File mpi_file;
    int mpi_error = MPI_File_open(mpi_comm, filename, yogi_amode_to_mpi(amode),
    		                      mpi_info, &mpi_file);
    if (mpi_error == MPI_SUCCESS) {
        *fh = add_new_file(mpi_file);
    }
    
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_close(YogiMPI_File *fh) {
    MPI_File mpi_file = file_to_mpi(*fh); 
    int mpi_error = MPI_File_close(&mpi_file);
    file_pool[*fh] = MPI_FILE_NULL;
    *fh = YogiMPI_FILE_NULL;
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_get_info(YogiMPI_File fh, YogiMPI_Info *info_used) {
	MPI_File mpi_file = file_to_mpi(fh);
	MPI_Info mpi_info;
    int mpi_error = MPI_File_get_info(mpi_file, &mpi_info); 	
    *info_used = add_new_info(mpi_info);
    
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_set_view(YogiMPI_File fh, YogiMPI_Offset disp, 
		                  YogiMPI_Datatype etype, YogiMPI_Datatype filetype,
                          const char *datarep, YogiMPI_Info info) {

	MPI_File mpi_file = file_to_mpi(fh);
	MPI_Offset mpi_offset = (MPI_Offset)disp;
	MPI_Datatype mpi_datatype = datatype_to_mpi(etype);
	MPI_Datatype mpi_datatype2 = datatype_to_mpi(filetype);
	MPI_Info mpi_info = info_to_mpi(info);
    int mpi_error = MPI_File_set_view(mpi_file, mpi_offset, mpi_datatype,
    		                          mpi_datatype2, datarep, mpi_info);    
    return error_to_yogi(mpi_error);
	
}

int YogiMPI_File_write_all(YogiMPI_File fh, const void *buf, int count, 
		                   YogiMPI_Datatype datatype, YogiMPI_Status *status) {
	MPI_File mpi_file = file_to_mpi(fh);
	MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
	int mpi_error;
    if (YogiMPI_STATUS_IGNORE != status) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_error = MPI_File_write_all(mpi_file, buf, count, mpi_datatype,
    		                           mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_error = MPI_File_write_all(mpi_file, buf, count, mpi_datatype,
    		                           MPI_STATUS_IGNORE);    	
    }
    
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_write_at(YogiMPI_File fh, YogiMPI_Offset offset, 
                          const void *buf, int count, YogiMPI_Datatype datatype,                          YogiMPI_Status *status) {
	
	MPI_File mpi_file = file_to_mpi(fh);
	/* Cast this to correct type. */
	MPI_Offset mpi_offset = (MPI_Offset)offset;
	int mpi_error;
	MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    if (YogiMPI_STATUS_IGNORE != status) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_error = MPI_File_write_at(mpi_file, mpi_offset, buf, count,
    		                          mpi_datatype, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_error = MPI_File_write_at(mpi_file, mpi_offset, buf, count,
    		                          mpi_datatype, MPI_STATUS_IGNORE);    	
    }
    
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_get_position(YogiMPI_File fh, YogiMPI_Offset *offset) {
    MPI_Offset input_offset;
    int mpi_error = MPI_File_get_position(file_to_mpi(fh), &input_offset);
    *offset = offset_to_yogi(input_offset);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Info_create(YogiMPI_Info *info) {
    MPI_Info mpi_info;
    int mpi_error = MPI_Info_create(&mpi_info);
    *info = add_new_info(mpi_info);
    
    return error_to_yogi(mpi_error);	
}

int YogiMPI_Info_set(YogiMPI_Info info, char *key, char *value) {
	MPI_Info mpi_info = info_to_mpi(info);
	int mpi_error = MPI_Info_set(mpi_info, key, value);
    
    return error_to_yogi(mpi_error);
}

int YogiMPI_Test(YogiMPI_Request *request, int *flag, YogiMPI_Status *status) {
    int mpi_error;
    MPI_Request* mpi_request = request_to_mpi(*request);
    if (YogiMPI_STATUS_IGNORE != status) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_error = MPI_Test(mpi_request, flag, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_error = MPI_Test(mpi_request, flag, MPI_STATUS_IGNORE);
    }

    if (MPI_REQUEST_NULL == *mpi_request) {
        assert(MPI_REQUEST_NULL == request_pool[*request]);
        *request = YogiMPI_REQUEST_NULL;
    }

    return error_to_yogi(mpi_error); 
}

int YogiMPI_Probe(int source, int tag, YogiMPI_Comm comm, 
                  YogiMPI_Status* status) {
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    int mpi_err;

    /* treat MPI_ANY_SOURCE */
    if (YogiMPI_ANY_SOURCE == source) source = MPI_ANY_SOURCE;

    /* treat MPI_ANY_TAG */
    if (YogiMPI_ANY_TAG == tag) tag = MPI_ANY_TAG;

    if (YogiMPI_STATUS_IGNORE != status) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_err = MPI_Probe(source, tag, mpi_comm, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_err = MPI_Probe(source, tag, mpi_comm, MPI_STATUS_IGNORE);
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Iprobe(int source, int tag, YogiMPI_Comm comm, int *flag,
                   YogiMPI_Status *status) {
    int mpi_error;
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    if (YogiMPI_ANY_SOURCE == source) source = MPI_ANY_SOURCE;
    if (YogiMPI_ANY_TAG == tag) tag = MPI_ANY_TAG;
    if (YogiMPI_STATUS_IGNORE != status) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_error = MPI_Iprobe(source, tag, mpi_comm, flag, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_error = MPI_Iprobe(source, tag, mpi_comm, flag, MPI_STATUS_IGNORE);
    }

    return error_to_yogi(mpi_error);
}

int YogiMPI_Abort(YogiMPI_Comm comm, int errorcode) {
    MPI_Comm mpi_comm = comm_to_mpi(comm);    
    int mpi_error = MPI_Abort(mpi_comm, errorcode);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Attr_get(YogiMPI_Comm comm, int keyval, void *attribute_val, 
                     int *flag) {
    if (keyval == YogiMPI_TAG_UB) {
        keyval = MPI_TAG_UB;
    }
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    int mpi_error = MPI_Attr_get(mpi_comm, keyval, attribute_val, flag);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Initialized(int *flag) {
    int mpi_error = MPI_Initialized(flag);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Finalized(int *flag) {
    int mpi_error = MPI_Finalized(flag);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Info_get_nthkey(YogiMPI_Info info, int n, char* key) {

    MPI_Info conv_info = info_to_mpi(info);
    int mpi_error;
    mpi_error = MPI_Info_get_nthkey(conv_info, n, key);
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_write(YogiMPI_File fh, void* buf, int count, 
		               YogiMPI_Datatype datatype, YogiMPI_Status* status) {

    MPI_File conv_fh = file_to_mpi(fh);
    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    int mpi_error;
    if (status == YogiMPI_STATUS_IGNORE) {
        mpi_error = MPI_File_write(conv_fh, buf, count, conv_datatype, 
        		                   MPI_STATUS_IGNORE);
    }
    else {
        MPI_Status * in_status = yogi_status_to_mpi(status);
        mpi_error = MPI_File_write(conv_fh, buf, count, conv_datatype, 
        		                   in_status);
        mpi_status_to_yogi(in_status, status);

    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Info_dup(YogiMPI_Info info, YogiMPI_Info* newinfo) {

    MPI_Info conv_info = info_to_mpi(info);
    MPI_Info conv_newinfo;
    int mpi_error;
    mpi_error = MPI_Info_dup(conv_info, &conv_newinfo);
    *newinfo = add_new_info(conv_newinfo);
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_read_at_all(YogiMPI_File mpi_fh, YogiMPI_Offset offset, void* buf, int count, YogiMPI_Datatype datatype, YogiMPI_Status* status) {

    MPI_File conv_mpi_fh = file_to_mpi(mpi_fh);
    MPI_Offset conv_offset = offset_to_mpi(offset);
    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    int mpi_error;
    if (status == YogiMPI_STATUS_IGNORE) {
        mpi_error = MPI_File_read_at_all(conv_mpi_fh, conv_offset, buf, count, conv_datatype, MPI_STATUS_IGNORE);
    }
    else {
        MPI_Status * in_status = yogi_status_to_mpi(status);
        mpi_error = MPI_File_read_at_all(conv_mpi_fh, conv_offset, buf, count, conv_datatype, in_status);
        mpi_status_to_yogi(in_status, status);

    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_seek(YogiMPI_File mpi_fh, YogiMPI_Offset offset, int whence) {

    MPI_File conv_mpi_fh = file_to_mpi(mpi_fh);
    MPI_Offset conv_offset = offset_to_mpi(offset);
    if (whence == YogiMPI_SEEK_SET) {
        whence = MPI_SEEK_SET;
    }
    else if (whence == YogiMPI_SEEK_CUR) {
        whence = MPI_SEEK_CUR;
    }
    else if (whence == YogiMPI_SEEK_END) {
        whence = MPI_SEEK_END;
    }
    int mpi_error;
    mpi_error = MPI_File_seek(conv_mpi_fh, conv_offset, whence);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_struct(int count, int blocklens[], YogiMPI_Aint indices[], YogiMPI_Datatype old_types[], YogiMPI_Datatype* newtype) {

    MPI_Aint * conv_indices = aint_array_to_mpi(indices, count);
    MPI_Datatype * conv_old_types = datatype_array_to_mpi(old_types, count);
    MPI_Datatype conv_newtype;
    int mpi_error;
    mpi_error = MPI_Type_struct(count, blocklens, conv_indices, conv_old_types, &conv_newtype);
    *newtype = add_new_datatype(conv_newtype);
    free_aint_array(conv_indices);
    free_datatype_array(conv_old_types);
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_read_at(YogiMPI_File mpi_fh, YogiMPI_Offset offset, void* buf, int count, YogiMPI_Datatype datatype, YogiMPI_Status* status) {

    MPI_File conv_mpi_fh = file_to_mpi(mpi_fh);
    MPI_Offset conv_offset = offset_to_mpi(offset);
    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    int mpi_error;
    if (status == YogiMPI_STATUS_IGNORE) {
        mpi_error = MPI_File_read_at(conv_mpi_fh, conv_offset, buf, count, conv_datatype, MPI_STATUS_IGNORE);
    }
    else {
        MPI_Status * in_status = yogi_status_to_mpi(status);
        mpi_error = MPI_File_read_at(conv_mpi_fh, conv_offset, buf, count, conv_datatype, in_status);
        mpi_status_to_yogi(in_status, status);

    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_write_at_all(YogiMPI_File mpi_fh, YogiMPI_Offset offset, void* buf, int count, YogiMPI_Datatype datatype, YogiMPI_Status* status) {

    MPI_File conv_mpi_fh = file_to_mpi(mpi_fh);
    MPI_Offset conv_offset = offset_to_mpi(offset);
    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    int mpi_error;
    if (status == YogiMPI_STATUS_IGNORE) {
        mpi_error = MPI_File_write_at_all(conv_mpi_fh, conv_offset, buf, count, conv_datatype, MPI_STATUS_IGNORE);
    }
    else {
        MPI_Status * in_status = yogi_status_to_mpi(status);
        mpi_error = MPI_File_write_at_all(conv_mpi_fh, conv_offset, buf, count, conv_datatype, in_status);
        mpi_status_to_yogi(in_status, status);

    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_extent(YogiMPI_Datatype datatype, YogiMPI_Aint* extent) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    MPI_Aint conv_extent;
    int mpi_error;
    mpi_error = MPI_Type_extent(conv_datatype, &conv_extent);
    *extent = aint_to_yogi(conv_extent);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Get_elements(YogiMPI_Status* status, YogiMPI_Datatype datatype, int* elements) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    int mpi_error;
    MPI_Status * in_status = yogi_status_to_mpi(status);
    mpi_error = MPI_Get_elements(in_status, conv_datatype, elements);
    if (*elements == MPI_UNDEFINED) {
        *elements = YogiMPI_UNDEFINED;
    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_hindexed(int count, int blocklens[], YogiMPI_Aint indices[], YogiMPI_Datatype old_type, YogiMPI_Datatype* newtype) {

    MPI_Aint * conv_indices = aint_array_to_mpi(indices, count);
    MPI_Datatype conv_old_type = datatype_to_mpi(old_type);
    MPI_Datatype conv_newtype;
    int mpi_error;
    mpi_error = MPI_Type_hindexed(count, blocklens, conv_indices, conv_old_type, &conv_newtype);
    *newtype = add_new_datatype(conv_newtype);
    free_aint_array(conv_indices);
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_set_atomicity(YogiMPI_File mpi_fh, int flag) {

    MPI_File conv_mpi_fh = file_to_mpi(mpi_fh);
    int mpi_error;
    mpi_error = MPI_File_set_atomicity(conv_mpi_fh, flag);
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_sync(YogiMPI_File mpi_fh) {

    MPI_File conv_mpi_fh = file_to_mpi(mpi_fh);
    int mpi_error;
    mpi_error = MPI_File_sync(conv_mpi_fh);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_hvector(int count, int blocklen, YogiMPI_Aint stride, YogiMPI_Datatype old_type, YogiMPI_Datatype* newtype) {

    MPI_Aint conv_stride = aint_to_mpi(stride);
    MPI_Datatype conv_old_type = datatype_to_mpi(old_type);
    MPI_Datatype conv_newtype;
    int mpi_error;
    mpi_error = MPI_Type_hvector(count, blocklen, conv_stride, conv_old_type, &conv_newtype);
    *newtype = add_new_datatype(conv_newtype);
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_delete(char* filename, YogiMPI_Info info) {

    MPI_Info conv_info = info_to_mpi(info);
    int mpi_error;
    mpi_error = MPI_File_delete(filename, conv_info);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Address(void* location, YogiMPI_Aint* address) {

    MPI_Aint conv_address;
    int mpi_error;
    mpi_error = MPI_Address(location, &conv_address);
    *address = aint_to_yogi(conv_address);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Info_free(YogiMPI_Info* info) {

    MPI_Info conv_info = info_to_mpi(*info);
    int mpi_error;
    mpi_error = MPI_Info_free(&conv_info);
    info_pool[*info] = MPI_INFO_NULL;
    *info = YogiMPI_INFO_NULL;
    return error_to_yogi(mpi_error);
}

int YogiMPI_Info_get(YogiMPI_Info info, char* key, int valuelen, char* value,
                     int* flag) {

    MPI_Info conv_info = info_to_mpi(info);
    int mpi_error = MPI_Info_get(conv_info, key, valuelen, value, flag);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Group_excl(YogiMPI_Group group, int n, int* ranks, 
                       YogiMPI_Group* newgroup) {

    MPI_Group conv_group = group_to_mpi(group);
    MPI_Group conv_newgroup;
    int mpi_error = MPI_Group_excl(conv_group, n, ranks, &conv_newgroup);
    *newgroup = add_new_group(conv_newgroup);
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_get_atomicity(YogiMPI_File mpi_fh, int* flag) {

    MPI_File conv_mpi_fh = file_to_mpi(mpi_fh);
    int mpi_error;
    mpi_error = MPI_File_get_atomicity(conv_mpi_fh, flag);
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_read(YogiMPI_File mpi_fh, void* buf, int count, YogiMPI_Datatype datatype, YogiMPI_Status* status) {

    MPI_File conv_mpi_fh = file_to_mpi(mpi_fh);
    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    int mpi_error;
    if (status == YogiMPI_STATUS_IGNORE) {
        mpi_error = MPI_File_read(conv_mpi_fh, buf, count, conv_datatype, MPI_STATUS_IGNORE);
    }
    else {
        MPI_Status * in_status = yogi_status_to_mpi(status);
        mpi_error = MPI_File_read(conv_mpi_fh, buf, count, conv_datatype, in_status);
        mpi_status_to_yogi(in_status, status);

    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Info_get_nkeys(YogiMPI_Info info, int* nkeys) {

    MPI_Info conv_info = info_to_mpi(info);
    int mpi_error;
    mpi_error = MPI_Info_get_nkeys(conv_info, nkeys);
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_get_size(YogiMPI_File fh, YogiMPI_Offset* size) {

    MPI_File conv_fh = file_to_mpi(fh);
    MPI_Offset conv_size;
    int mpi_error;
    mpi_error = MPI_File_get_size(conv_fh, &conv_size);
    *size = offset_to_yogi(conv_size);
    return error_to_yogi(mpi_error);
}

int YogiMPI_File_set_size(YogiMPI_File fh, YogiMPI_Offset size) {

    MPI_File conv_fh = file_to_mpi(fh);
    MPI_Offset conv_size = offset_to_mpi(size);
    int mpi_error;
    mpi_error = MPI_File_set_size(conv_fh, conv_size);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Error_string(int errorcode, char* string, int* resultlen) {

    int mpi_error;
    mpi_error = MPI_Error_string(errorcode, string, resultlen);
    return error_to_yogi(mpi_error);
}

YogiMPI_Comm YogiMPI_Comm_f2c(YogiMPI_Fint comm) {
	return comm;
}

YogiMPI_Fint YogiMPI_Comm_c2f(YogiMPI_Comm comm) {
	return comm;
}

YogiMPI_Info YogiMPI_Info_f2c(YogiMPI_Fint info) {
	return info;
}

YogiMPI_Fint YogiMPI_Info_c2f(YogiMPI_Info info) {
	return info;
}

YogiMPI_Datatype YogiMPI_Type_f2c(YogiMPI_Fint type) {
	return type;
}

int YogiMPI_File_read_all(YogiMPI_File mpi_fh, void* buf, int count, 
		                  YogiMPI_Datatype datatype, YogiMPI_Status* status) {

    MPI_File conv_mpi_fh = file_to_mpi(mpi_fh);
    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    int mpi_error;
    if (status == YogiMPI_STATUS_IGNORE) {
        mpi_error = MPI_File_read_all(conv_mpi_fh, buf, count, conv_datatype, 
        		                      MPI_STATUS_IGNORE);
    }
    else {
        MPI_Status * in_status = yogi_status_to_mpi(status);
        mpi_error = MPI_File_read_all(conv_mpi_fh, buf, count, conv_datatype,
        		                      in_status);
        mpi_status_to_yogi(in_status, status);

    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_get_extent(YogiMPI_Datatype datatype, YogiMPI_Aint* lb,
		                    YogiMPI_Aint* extent) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    MPI_Aint conv_lb;
    MPI_Aint conv_extent;
    int mpi_error;
    mpi_error = MPI_Type_get_extent(conv_datatype, &conv_lb, &conv_extent);
    if (*lb == MPI_UNDEFINED) {
        *lb = YogiMPI_UNDEFINED;
    }
    if (*extent == MPI_UNDEFINED) {
        *extent = YogiMPI_UNDEFINED;
    }
    *lb = aint_to_yogi(conv_lb);
    *extent = aint_to_yogi(conv_extent);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Cancel(YogiMPI_Request* request) {

    MPI_Request *conv_request = request_to_mpi(*request);
    int mpi_error;
    mpi_error = MPI_Cancel(conv_request);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Attr_put(YogiMPI_Comm comm, int keyval, void* attr_value) {

    MPI_Comm conv_comm = comm_to_mpi(comm);
    int mpi_error;
    mpi_error = MPI_Attr_put(conv_comm, keyval, attr_value);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Buffer_attach(void* buffer, int size) {

    int mpi_error;
    mpi_error = MPI_Buffer_attach(buffer, size);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Rsend(const void* buf, int count, YogiMPI_Datatype datatype,
                  int dest, int tag, YogiMPI_Comm comm) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    MPI_Comm conv_comm = comm_to_mpi(comm);
    int mpi_error;
    mpi_error = MPI_Rsend(buf, count, conv_datatype, dest, tag, conv_comm);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Unpack(void* inbuf, int insize, int* position, void* outbuf, 
                   int outcount, YogiMPI_Datatype datatype, YogiMPI_Comm comm) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    MPI_Comm conv_comm = comm_to_mpi(comm);
    int mpi_error;
    mpi_error = MPI_Unpack(inbuf, insize, position, outbuf, outcount, 
    		               conv_datatype, conv_comm);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Buffer_detach(void* buffer, int* size) {

    int mpi_error;
    mpi_error = MPI_Buffer_detach(buffer, size);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Bsend(const void* buf, int count, YogiMPI_Datatype datatype,
                  int dest, int tag, YogiMPI_Comm comm) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    MPI_Comm conv_comm = comm_to_mpi(comm);
    int mpi_error;
    mpi_error = MPI_Bsend(buf, count, conv_datatype, dest, tag, conv_comm);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Pack_size(int incount, YogiMPI_Datatype datatype, YogiMPI_Comm comm,
		              int* size) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    MPI_Comm conv_comm = comm_to_mpi(comm);
    int mpi_error;
    mpi_error = MPI_Pack_size(incount, conv_datatype, conv_comm, size);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Intercomm_create(YogiMPI_Comm local_comm, int local_leader,
                             YogiMPI_Comm peer_comm, int remote_leader, int tag,
                             YogiMPI_Comm* newintercomm) {

    MPI_Comm conv_local_comm = comm_to_mpi(local_comm);
    MPI_Comm conv_peer_comm = comm_to_mpi(peer_comm);
    MPI_Comm conv_newintercomm;
    int mpi_error;
    mpi_error = MPI_Intercomm_create(conv_local_comm, local_leader, 
                                     conv_peer_comm, remote_leader, tag, 
                                     &conv_newintercomm);
    *newintercomm = add_new_comm(conv_newintercomm);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Keyval_free(int* keyval) {

    int mpi_error;
    mpi_error = MPI_Keyval_free(keyval);
    if (*keyval == MPI_KEYVAL_INVALID) {
        *keyval = YogiMPI_KEYVAL_INVALID;
    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Pack(void* inbuf, int incount, YogiMPI_Datatype datatype, 
                 void* outbuf, int outcount, int* position, YogiMPI_Comm comm) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    MPI_Comm conv_comm = comm_to_mpi(comm);
    int mpi_error;
    mpi_error = MPI_Pack(inbuf, incount, conv_datatype, outbuf, outcount, 
    		             position, conv_comm);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Group_range_excl(YogiMPI_Group group, int n, int ranges[][3], 
                             YogiMPI_Group* newgroup) {

    MPI_Group conv_group = group_to_mpi(group);
    MPI_Group conv_newgroup;
    int mpi_error = MPI_Group_range_excl(conv_group, n, ranges,
    		                         &conv_newgroup);
    *newgroup = add_new_group(conv_newgroup);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_create_hindexed(int count, int blocklengths[],
                                 YogiMPI_Aint displacements[],
                                 YogiMPI_Datatype oldtype,
                                 YogiMPI_Datatype* newtype) {

    MPI_Aint *conv_displacements = aint_array_to_mpi(displacements, count);
    MPI_Datatype conv_oldtype = datatype_to_mpi(oldtype);
    MPI_Datatype conv_newtype;
    int mpi_error = MPI_Type_create_hindexed(count, blocklengths, 
                                         conv_displacements, conv_oldtype, 
                                         &conv_newtype);
    *newtype = add_new_datatype(conv_newtype);
    free_aint_array(conv_displacements);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Pcontrol(int level) {

    int mpi_error;
    mpi_error = MPI_Pcontrol(level);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Error_class(int errorcode, int* errorclass) {

    *errorclass = errorcode;
    return error_to_yogi(MPI_SUCCESS);
}

int YogiMPI_Irsend(const void* buf, int count, YogiMPI_Datatype datatype,
                   int dest, int tag, YogiMPI_Comm comm, 
                   YogiMPI_Request* request) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    MPI_Comm conv_comm = comm_to_mpi(comm);
    MPI_Request conv_request;
    int mpi_error;
    mpi_error = MPI_Irsend(buf, count, conv_datatype, dest, tag, conv_comm,
    		               &conv_request);
    *request = add_new_request(conv_request);
    return error_to_yogi(mpi_error);
}

double YogiMPI_Wtick() {
    return MPI_Wtick();
}

int YogiMPI_Type_create_struct(int count, int array_of_blocklengths[],
                               YogiMPI_Aint array_of_displacements[],
                               YogiMPI_Datatype array_of_types[],
                               YogiMPI_Datatype* newtype) {

    MPI_Aint *conv_array_of_displacements = 
    		                   aint_array_to_mpi(array_of_displacements, count);
    MPI_Datatype *conv_array_of_types = datatype_array_to_mpi(array_of_types,
                                                              count);
    MPI_Datatype conv_newtype;
    int mpi_error;
    mpi_error = MPI_Type_create_struct(count, array_of_blocklengths,
                                       conv_array_of_displacements,
                                       conv_array_of_types, &conv_newtype);
    *newtype = add_new_datatype(conv_newtype);
    free_aint_array(conv_array_of_displacements);
    free_datatype_array(conv_array_of_types);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Reduce_scatter(const void* sendbuf, void* recvbuf, int* recvcnts,
                           YogiMPI_Datatype datatype, YogiMPI_Op op, 
                           YogiMPI_Comm comm) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    MPI_Op conv_op = op_to_mpi(op);
    MPI_Comm conv_comm = comm_to_mpi(comm);

    int mpi_err;
    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Reduce_scatter(MPI_IN_PLACE, recvbuf, recvcnts,
                                     conv_datatype, conv_op, conv_comm);
    }
    else {
        mpi_err = MPI_Reduce_scatter(sendbuf, recvbuf, recvcnts, conv_datatype,
                                     conv_op, conv_comm);
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_compare(YogiMPI_Comm comm1, YogiMPI_Comm comm2, int* result) {

    MPI_Comm conv_comm1 = comm_to_mpi(comm1);
    MPI_Comm conv_comm2 = comm_to_mpi(comm2);
    int mpi_error;
    mpi_error = MPI_Comm_compare(conv_comm1, conv_comm2, result);
    if (*result == MPI_IDENT) {
        *result = YogiMPI_IDENT;
    }
    else if (*result == MPI_CONGRUENT) {
        *result = YogiMPI_CONGRUENT;
    }
    else if (*result == MPI_SIMILAR) {
        *result = YogiMPI_SIMILAR;
    }
    else if (*result == MPI_UNEQUAL) {
        *result = YogiMPI_UNEQUAL;
    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Sendrecv(const void* sendbuf, int sendcount, 
                     YogiMPI_Datatype sendtype, int dest, int sendtag,
                     void* recvbuf, int recvcount, YogiMPI_Datatype recvtype,
                     int source, int recvtag, YogiMPI_Comm comm,
                     YogiMPI_Status* status) {

    MPI_Datatype conv_sendtype = datatype_to_mpi(sendtype);
    MPI_Datatype conv_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm conv_comm = comm_to_mpi(comm);
    int mpi_error;
    if (status == YogiMPI_STATUS_IGNORE) {
        mpi_error = MPI_Sendrecv(sendbuf, sendcount, conv_sendtype, dest, 
                                 sendtag, recvbuf, recvcount, conv_recvtype,
                                 source, recvtag, conv_comm, MPI_STATUS_IGNORE);
    }
    else {
        MPI_Status * in_status = yogi_status_to_mpi(status);
        mpi_error = MPI_Sendrecv(sendbuf, sendcount, conv_sendtype, dest,
                                 sendtag, recvbuf, recvcount, conv_recvtype,
                                 source, recvtag, conv_comm, in_status);
        mpi_status_to_yogi(in_status, status);
    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Group_size(YogiMPI_Group group, int* size) {

    MPI_Group conv_group = group_to_mpi(group);
    int mpi_error;
    mpi_error = MPI_Group_size(conv_group, size);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_create_subarray(int ndims, int array_of_sizes[], 
                                 int array_of_subsizes[], int array_of_starts[],
                                 int order, YogiMPI_Datatype oldtype,
                                 YogiMPI_Datatype* newtype) {

    MPI_Datatype conv_oldtype = datatype_to_mpi(oldtype);
    MPI_Datatype conv_newtype;
    if (order == YogiMPI_ORDER_C) {
        order = MPI_ORDER_C;
    }
    else if (order == YogiMPI_ORDER_FORTRAN) {
        order = MPI_ORDER_FORTRAN;
    }
    int mpi_error;
    mpi_error = MPI_Type_create_subarray(ndims, array_of_sizes, 
    		                             array_of_subsizes, array_of_starts, 
										 order, conv_oldtype, &conv_newtype);
    *newtype = add_new_datatype(conv_newtype);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Attr_delete(YogiMPI_Comm comm, int keyval) {

    MPI_Comm conv_comm = comm_to_mpi(comm);
    int mpi_error;
    mpi_error = MPI_Attr_delete(conv_comm, keyval);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Comm_remote_size(YogiMPI_Comm comm, int* size) {

    MPI_Comm conv_comm = comm_to_mpi(comm);
    int mpi_error;
    mpi_error = MPI_Comm_remote_size(conv_comm, size);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Get_address(void* location, YogiMPI_Aint* address) {

    MPI_Aint conv_address;
    int mpi_error;
    mpi_error = MPI_Get_address(location, &conv_address);
    *address = aint_to_yogi(conv_address);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_create_hvector(int count, int blocklength, YogiMPI_Aint stride,
                                YogiMPI_Datatype oldtype,
                                YogiMPI_Datatype* newtype) {

    MPI_Aint conv_stride = aint_to_mpi(stride);
    MPI_Datatype conv_oldtype = datatype_to_mpi(oldtype);
    MPI_Datatype conv_newtype;
    int mpi_error;
    mpi_error = MPI_Type_create_hvector(count, blocklength, conv_stride,
                                        conv_oldtype, &conv_newtype);
    *newtype = add_new_datatype(conv_newtype);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Errhandler_free(YogiMPI_Errhandler* errhandler) {

    MPI_Errhandler conv_errhandler = errhandler_to_mpi(*errhandler);
    int mpi_error;
    mpi_error = MPI_Errhandler_free(&conv_errhandler);
    errhandler_pool[*errhandler] = MPI_ERRHANDLER_NULL;
    *errhandler = YogiMPI_ERRHANDLER_NULL;
    return error_to_yogi(mpi_error);
}

int YogiMPI_Comm_get_errhandler(YogiMPI_Comm comm,
		                        YogiMPI_Errhandler* errhandler) {

    MPI_Comm conv_comm = comm_to_mpi(comm);
    MPI_Errhandler conv_errhandler;
    int mpi_error;
    mpi_error = MPI_Comm_get_errhandler(conv_comm, &conv_errhandler);
    
	if (conv_errhandler == MPI_ERRHANDLER_NULL) {
		*errhandler = YogiMPI_ERRHANDLER_NULL;
	}
	else if (conv_errhandler == MPI_ERRORS_ARE_FATAL) {
		*errhandler = YogiMPI_ERRORS_ARE_FATAL;
	}
	else if (conv_errhandler == MPI_ERRORS_RETURN) {
		*errhandler = YogiMPI_ERRORS_RETURN;
	}
	else {
	    *errhandler = add_new_errhandler(conv_errhandler);		
	}
    return error_to_yogi(mpi_error);
}

int YogiMPI_Comm_set_errhandler(YogiMPI_Comm comm,
		                        YogiMPI_Errhandler errhandler) {

    MPI_Comm conv_comm = comm_to_mpi(comm);
    MPI_Errhandler conv_errhandler = errhandler_to_mpi(errhandler);
    int mpi_error;
    mpi_error = MPI_Comm_set_errhandler(conv_comm, conv_errhandler);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Errhandler_set(YogiMPI_Comm comm, YogiMPI_Errhandler errhandler) {

    MPI_Comm conv_comm = comm_to_mpi(comm);
    MPI_Errhandler conv_errhandler = errhandler_to_mpi(errhandler);
    int mpi_error;
    mpi_error = MPI_Errhandler_set(conv_comm, conv_errhandler);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_get_envelope(YogiMPI_Datatype datatype, int* num_integers,
		                      int* num_addresses, int* num_datatypes,
							  int* combiner) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    int mpi_error;
    mpi_error = MPI_Type_get_envelope(conv_datatype, num_integers, 
    		                          num_addresses, num_datatypes, combiner);
    
    switch (*combiner) {
        case MPI_COMBINER_NAMED:
        	*combiner = YogiMPI_COMBINER_NAMED;
        	break;
        case MPI_COMBINER_DUP:
    	    *combiner = YogiMPI_COMBINER_DUP;        
            break;
        case MPI_COMBINER_CONTIGUOUS:
    	    *combiner = YogiMPI_COMBINER_CONTIGUOUS;        
        	break;
        case MPI_COMBINER_VECTOR:
    	    *combiner = YogiMPI_COMBINER_VECTOR;
        	break;
        case MPI_COMBINER_HVECTOR_INTEGER:
    	    *combiner = YogiMPI_COMBINER_HVECTOR_INTEGER;
        	break;
        case MPI_COMBINER_HVECTOR:
    	    *combiner = YogiMPI_COMBINER_HVECTOR;
        	break;
        case MPI_COMBINER_INDEXED:
        	*combiner = YogiMPI_COMBINER_INDEXED;
        	break;
        case MPI_COMBINER_HINDEXED_INTEGER:
        	*combiner = YogiMPI_COMBINER_HINDEXED_INTEGER;
        	break;
        case MPI_COMBINER_HINDEXED:
        	*combiner = YogiMPI_COMBINER_HINDEXED;
        	break;
        case MPI_COMBINER_INDEXED_BLOCK:
        	*combiner = YogiMPI_COMBINER_INDEXED_BLOCK;
        	break;
        case MPI_COMBINER_STRUCT_INTEGER:
        	*combiner = YogiMPI_COMBINER_STRUCT_INTEGER;
        	break;
        case MPI_COMBINER_STRUCT:
		    *combiner = YogiMPI_COMBINER_STRUCT;
         	break;
        case MPI_COMBINER_SUBARRAY:
        	*combiner - YogiMPI_COMBINER_SUBARRAY;
        	break;
        case MPI_COMBINER_DARRAY:
        	*combiner = YogiMPI_COMBINER_DARRAY;
        	break;
        case MPI_COMBINER_F90_REAL:
        	*combiner = YogiMPI_COMBINER_F90_REAL;
        	break;
        case MPI_COMBINER_F90_COMPLEX:
        	*combiner = YogiMPI_COMBINER_F90_COMPLEX;
        	break;
        case MPI_COMBINER_F90_INTEGER:
        	*combiner = YogiMPI_COMBINER_F90_INTEGER;
        	break;
        case MPI_COMBINER_RESIZED:
        	*combiner = YogiMPI_COMBINER_RESIZED;
        	break;
    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_get_contents(YogiMPI_Datatype datatype, int max_integers,
                             int max_addresses, int max_datatypes,
                             int array_of_integers[],
                             YogiMPI_Aint array_of_addresses[],
                             YogiMPI_Datatype array_of_datatypes[]) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);

    /* In order to appropriately handle this function, we must call 
     * MPI_Type_get_envelope to figure out what the user will get. This helps
     * determine if the type is derived and if new datatype handles are added
     * to our translation pool.
     */
    int num_integers, num_addresses, num_datatypes, combiner;
    MPI_Type_get_envelope(conv_datatype, &num_integers, &num_addresses, 
    		              &num_datatypes, &combiner);
    /* This should not ever be a named, predefined data type. Bad call. */
    assert(combiner != MPI_COMBINER_NAMED);
    
    MPI_Aint * conv_array_of_addresses = 
    		           (MPI_Aint*)malloc(max_addresses*sizeof(MPI_Aint));
    MPI_Datatype * conv_array_of_datatypes =
    		          (MPI_Datatype*)malloc(max_datatypes*sizeof(MPI_Datatype));
    int mpi_error;
    mpi_error = MPI_Type_get_contents(conv_datatype, max_integers,
                                      max_addresses, max_datatypes,
				      array_of_integers,
                                      conv_array_of_addresses,
                                      conv_array_of_datatypes);
    aint_array_to_yogi(conv_array_of_addresses, array_of_addresses,
    		           max_addresses);
    if (combiner != MPI_COMBINER_F90_COMPLEX &&
        combiner != MPI_COMBINER_F90_REAL && 
		combiner != MPI_COMBINER_F90_INTEGER) {
    	/* The array won't be empty, so convert them to Yogi versions. */
        datatype_array_to_yogi(conv_array_of_datatypes, array_of_datatypes,
    		                   max_datatypes);
    }
    free_aint_array(conv_array_of_addresses);
    free_datatype_array(conv_array_of_datatypes);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Op_free(YogiMPI_Op* op) {

    MPI_Op conv_op = op_to_mpi(*op);
    int mpi_error;
    mpi_error = MPI_Op_free(&conv_op);
    op_pool[*op] = MPI_OP_NULL;
    *op = YogiMPI_OP_NULL;
    return error_to_yogi(mpi_error);
}

int YogiMPI_Cart_coords(YogiMPI_Comm comm, int rank, int maxdims, int coords[])
{

    MPI_Comm conv_comm = comm_to_mpi(comm);
    int mpi_error;
    mpi_error = MPI_Cart_coords(conv_comm, rank, maxdims, coords);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Cart_create(YogiMPI_Comm comm_old, int ndims, int dims[],
                        int periods[], int reorder, YogiMPI_Comm* comm_cart) {

    MPI_Comm conv_comm_old = comm_to_mpi(comm_old);
    MPI_Comm conv_comm_cart;
    int mpi_error;
    mpi_error = MPI_Cart_create(conv_comm_old, ndims, dims, periods, reorder,
                                &conv_comm_cart);
    if (conv_comm_cart != MPI_COMM_NULL) {
        *comm_cart = add_new_comm(conv_comm_cart);
    }
    else {
    	*comm_cart = YogiMPI_COMM_NULL;
    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Dims_create(int nnodes, int ndims, int dims[]) {

    int mpi_error;
    mpi_error = MPI_Dims_create(nnodes, ndims, dims);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Info_get_valuelen(YogiMPI_Info info, char* key, int* valuelen,
                              int* flag) {

    MPI_Info conv_info = info_to_mpi(info);
    int mpi_error;
    mpi_error = MPI_Info_get_valuelen(conv_info, key, valuelen, flag);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_create_darray(int size, int rank, int ndims,
                               int array_of_gsizes[], int array_of_distribs[],
                               int array_of_dargs[], int array_of_psizes[],
                               int order, YogiMPI_Datatype oldtype,
                               YogiMPI_Datatype* newtype) {

    MPI_Datatype conv_oldtype = datatype_to_mpi(oldtype);
    MPI_Datatype conv_newtype;
    /* Constant conversion requires some temporary arrays to hold state
     * and distribution values.
     */
    int distribs_conv[ndims];
    int dargs_conv[ndims];
    int i;
    for (i = 0; i < ndims; i++) {
    	switch(array_of_distribs[i]) {
    	    case YogiMPI_DISTRIBUTE_BLOCK:
    	    	distribs_conv[i] = MPI_DISTRIBUTE_BLOCK;
                break;
    	    case YogiMPI_DISTRIBUTE_CYCLIC:
    	    	distribs_conv[i] = MPI_DISTRIBUTE_CYCLIC;
    		    break;
    	    case YogiMPI_DISTRIBUTE_NONE:
    	    	distribs_conv[i] = MPI_DISTRIBUTE_NONE;
    		    break;
    	    default:
    		    distribs_conv[i] = array_of_distribs[i];
    	}
    	if (array_of_dargs[i] == YogiMPI_DISTRIBUTE_DFLT_DARG) {
    	    dargs_conv[i] = MPI_DISTRIBUTE_DFLT_DARG;
    	}
    	else {
    		dargs_conv[i] = array_of_dargs[i];
    	}
    }
    int mpi_error;
    mpi_error = MPI_Type_create_darray(size, rank, ndims, array_of_gsizes,
                                       distribs_conv, dargs_conv,
                                       array_of_psizes, order, conv_oldtype,
                                       &conv_newtype);
    *newtype = add_new_datatype(conv_newtype);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_create_resized(YogiMPI_Datatype oldtype, YogiMPI_Aint lb,
                                YogiMPI_Aint extent, YogiMPI_Datatype* newtype)
{

    MPI_Datatype conv_oldtype = datatype_to_mpi(oldtype);
    MPI_Aint conv_lb = aint_to_mpi(lb);
    MPI_Aint conv_extent = aint_to_mpi(extent);
    MPI_Datatype conv_newtype;
    int mpi_error;
    mpi_error = MPI_Type_create_resized(conv_oldtype, conv_lb, conv_extent,
    		                            &conv_newtype);
    *newtype = add_new_datatype(conv_newtype);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Keyval_create(YogiMPI_Copy_function* copy_fn,
                          YogiMPI_Delete_function* delete_fn, int* keyval,
                          void* extra_state) {

    MPI_Copy_function* mpi_copy_fn;
    MPI_Delete_function* mpi_delete_fn;
    if (copy_fn == YogiMPI_NULL_COPY_FN) {
        mpi_copy_fn = MPI_NULL_COPY_FN;
    }
    else if (copy_fn == YogiMPI_DUP_FN) {
        mpi_copy_fn = MPI_DUP_FN;
    }
    else {
    	mpi_copy_fn = (MPI_Copy_function*)copy_fn;
    }
    if (delete_fn == YogiMPI_NULL_DELETE_FN) {
        mpi_delete_fn = MPI_NULL_DELETE_FN;
    }
    else {
    	mpi_delete_fn = (MPI_Delete_function*)delete_fn;
    }
    int mpi_error;
    mpi_error = MPI_Keyval_create(mpi_copy_fn, mpi_delete_fn, keyval,
                                  extra_state);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Comm_create_errhandler(YogiMPI_Comm_errhandler_function* function,
                                   YogiMPI_Errhandler* errhandler) {

    MPI_Errhandler conv_errhandler;
    MPI_Comm_errhandler_function * mpi_function = 
    		                            (MPI_Comm_errhandler_function*)function;
    int mpi_error;
    mpi_error = MPI_Comm_create_errhandler(mpi_function, &conv_errhandler);
    *errhandler = add_new_errhandler(conv_errhandler);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Op_create(YogiMPI_User_function* function, int commute,
                      YogiMPI_Op* op) {

    MPI_Op conv_op;
    MPI_User_function * mpi_function = (MPI_User_function*)function;
    int mpi_error;
    mpi_error = MPI_Op_create(mpi_function, commute, &conv_op);
    *op = add_new_op(conv_op);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Comm_call_errhandler(YogiMPI_Comm comm, int errorcode) {
	MPI_Comm comm_conv = comm_to_mpi(comm);
	int mpi_error = MPI_Comm_call_errhandler(comm_conv, yogi_error_to_mpi(errorcode));
	return error_to_yogi(mpi_error);
}

int YogiMPI_Testall(int count, YogiMPI_Request array_of_requests[],
                    int *flag, YogiMPI_Status array_of_statuses[]) {

    int i, mpi_error;
    MPI_Request mpi_requests[count];
    for(i = 0; i < count; ++i) {
        mpi_requests[i] = *request_to_mpi(array_of_requests[i]);
    }    
    if (YogiMPI_STATUSES_IGNORE != array_of_statuses) {
        MPI_Status mpi_statuses[count];
        for (i = 0; i < count; i++) {
            mpi_statuses[i] = *(yogi_status_to_mpi(&array_of_statuses[i]));
        }
        mpi_error = MPI_Testall(count, mpi_requests, flag, mpi_statuses);
        for(i = 0; i < count; i++) {
            mpi_status_to_yogi(&mpi_statuses[i], &array_of_statuses[i]);
        }
    }
    else {
        mpi_error = MPI_Testall(count, mpi_requests, flag, MPI_STATUSES_IGNORE);
    }
    /* reset requests */
    for(i = 0; i < count; ++i) {
        if(mpi_requests[i] == MPI_REQUEST_NULL) {
            *request_to_mpi(array_of_requests[i]) = MPI_REQUEST_NULL;
            array_of_requests[i] = YogiMPI_REQUEST_NULL;
        }
    }

    return error_to_yogi(mpi_error);
}

int YogiMPI_Test_cancelled(YogiMPI_Status *status, int *flag) {
    MPI_Status *mpi_status = yogi_status_to_mpi(status);
    int mpi_err = MPI_Test_cancelled(mpi_status, flag);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Group_range_incl(YogiMPI_Group group, int n, int ranges[][3],
                             YogiMPI_Group *newgroup) {
    MPI_Group mpi_group = group_to_mpi(group);
    MPI_Group mpi_newgroup;
    int mpi_error = MPI_Group_range_incl(mpi_group, n, ranges, &mpi_newgroup);
    *newgroup = add_new_group(mpi_newgroup);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Testany(int count, YogiMPI_Request *array_of_requests,
                    int *index, int *flag, YogiMPI_Status *status) {

    int i;
    int mpi_err;
    MPI_Request* mpi_requests = (MPI_Request*)malloc(count*sizeof(MPI_Request));
    for(i = 0; i < count; ++i) {
        mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }

    if (YogiMPI_STATUS_IGNORE != status ) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_err = MPI_Testany(count, mpi_requests, index, flag, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_err = MPI_Testany(count, mpi_requests, index, flag,
                              MPI_STATUS_IGNORE);
    }

    /* reset requests */
    if (*index == MPI_UNDEFINED) {
        *index = YogiMPI_UNDEFINED;
    }
    else {
        if(mpi_requests[*index] == MPI_REQUEST_NULL) {
            *request_to_mpi(array_of_requests[*index]) = MPI_REQUEST_NULL;
            array_of_requests[*index] = YogiMPI_REQUEST_NULL;
        }
    }
    free(mpi_requests);
    return error_to_yogi(mpi_err);

}

int YogiMPI_Testsome(int incount, YogiMPI_Request *array_of_requests,
                     int *outcount, int *array_of_indices, 
                     YogiMPI_Status *array_of_statuses) {

    int i;
    int mpi_err;
    MPI_Request* mpi_requests = (MPI_Request*)malloc(incount*sizeof(MPI_Request));
    for(i = 0; i < incount; ++i) {
        mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }

    if (YogiMPI_STATUSES_IGNORE != array_of_statuses ) {
        MPI_Status* mpi_statuses = (MPI_Status*)malloc(incount*sizeof(MPI_Status));
        for (i = 0; i < incount; i++) {
            mpi_statuses[i] = *(yogi_status_to_mpi(&array_of_statuses[i]));
        }
            mpi_err = MPI_Testsome(incount, mpi_requests, outcount,
                                   array_of_indices, mpi_statuses);
        for(i = 0; i < *outcount; i++) {
            mpi_status_to_yogi(&mpi_statuses[i], &array_of_statuses[i]);
        }
        free(mpi_statuses);
    }
    else {
        mpi_err = MPI_Testsome(incount, mpi_requests, outcount,
                               array_of_indices, MPI_STATUSES_IGNORE);
    }

    /* reset requests */
    if (*outcount == MPI_UNDEFINED) {
        *outcount = YogiMPI_UNDEFINED;
    }
    else {
        for(i = 0; i < *outcount; ++i) {
            int requestIndex = array_of_indices[i];
            if(mpi_requests[requestIndex] == MPI_REQUEST_NULL) {
                *request_to_mpi(array_of_requests[requestIndex]) = MPI_REQUEST_NULL;
                array_of_requests[requestIndex] = YogiMPI_REQUEST_NULL;
            }
        }
    }
    free(mpi_requests);
    return error_to_yogi(mpi_err);

}

int YogiMPI_Comm_create_keyval(YogiMPI_Comm_copy_attr_function
                               *comm_copy_attr_fn,
                               YogiMPI_Comm_delete_attr_function
                               *comm_delete_attr_fn, int *comm_keyval,
                               void *extra_state) {
    MPI_Comm_copy_attr_function * copy_function;
    MPI_Comm_delete_attr_function * delete_function;
    if (comm_copy_attr_fn == YogiMPI_COMM_NULL_COPY_FN) {
        copy_function = MPI_COMM_NULL_COPY_FN;
    }
    else if (comm_copy_attr_fn == YogiMPI_COMM_DUP_FN) {
        copy_function = MPI_COMM_DUP_FN;
    }
    else {
        copy_function = (MPI_Comm_copy_attr_function*)comm_copy_attr_fn;
    }
    if (comm_delete_attr_fn == YogiMPI_COMM_NULL_DELETE_FN) {
        delete_function = MPI_COMM_NULL_DELETE_FN;
    }
    else {
        delete_function = (MPI_Comm_delete_attr_function*)comm_delete_attr_fn;
    }
    int mpi_err = MPI_Comm_create_keyval(copy_function, delete_function,
                                         comm_keyval, extra_state);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_free_keyval(int *comm_keyval) {
    int mpi_err = MPI_Comm_free_keyval(comm_keyval);
    if (*comm_keyval == MPI_KEYVAL_INVALID) {
        *comm_keyval = YogiMPI_KEYVAL_INVALID;
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_set_attr(YogiMPI_Comm comm, int comm_keyval, 
                          void *attribute_val) {
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    int mpi_err = MPI_Comm_set_attr(mpi_comm, comm_keyval, attribute_val);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_delete_attr(YogiMPI_Comm comm, int comm_keyval) {
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    int mpi_err = MPI_Comm_delete_attr(mpi_comm, comm_keyval);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_get_attr(YogiMPI_Comm comm, int comm_keyval, 
                          void *attribute_val, int *flag) {
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    int mpi_err = MPI_Comm_get_attr(mpi_comm, comm_keyval, attribute_val, flag);
    return error_to_yogi(mpi_err);

}

int YogiMPI_Type_dup(YogiMPI_Datatype type, YogiMPI_Datatype *newtype) {

    MPI_Datatype mpi_oldtype = datatype_to_mpi(type);
    MPI_Datatype mpi_newtype;

    int mpi_err = MPI_Type_dup(mpi_oldtype, &mpi_newtype);

    *newtype = add_new_datatype(mpi_newtype);

    return error_to_yogi(mpi_err);

}

int YogiMPI_Type_get_true_extent(YogiMPI_Datatype datatype, 
                                 YogiMPI_Aint *true_lb,
                                 YogiMPI_Aint *true_extent) {
    MPI_Aint conv_lb;
    MPI_Aint conv_extent;
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);

    int mpi_err = MPI_Type_get_true_extent(mpi_datatype, &conv_lb,
                                           &conv_extent);
    if (conv_lb == MPI_UNDEFINED) *true_lb = YogiMPI_UNDEFINED; 
    else *true_lb = aint_to_yogi(conv_lb);
    if (conv_extent == MPI_UNDEFINED) *true_extent = YogiMPI_UNDEFINED; 
    else *true_extent = aint_to_yogi(conv_extent);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Alloc_mem(YogiMPI_Aint size, YogiMPI_Info info, void *baseptr) {
    return error_to_yogi(MPI_Alloc_mem(aint_to_mpi(size), info_to_mpi(info),
                         baseptr));
}

int YogiMPI_Free_mem(void *base) {
    return error_to_yogi(MPI_Free_mem(base));
}

int YogiMPI_Exscan(const void *sendbuf, void *recvbuf, int count,
                   YogiMPI_Datatype datatype, YogiMPI_Op op, 
                   YogiMPI_Comm comm) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    MPI_Op conv_op = op_to_mpi(op);
    MPI_Comm conv_comm = comm_to_mpi(comm);

    int mpi_err;

    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Exscan(MPI_IN_PLACE, recvbuf, count, conv_datatype,
                             conv_op, conv_comm);
    }
    else {
        mpi_err = MPI_Exscan(sendbuf, recvbuf, count, conv_datatype, conv_op,
                             conv_comm);
    }

    return error_to_yogi(mpi_err);

}

