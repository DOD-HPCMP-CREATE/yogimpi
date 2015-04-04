#include <yogimpi.h>
#include <mpi.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> /* for hostname */

/* Define some Fortran datatypes not used in C.
 * Don't declare these in the header since C MPI apps should not use them,
 * but YogiMPI must know about them internally to provide a translation array.
 */
static const YogiMPI_Datatype YogiMPI_COMPLEX = 23;
static const YogiMPI_Datatype YogiMPI_DOUBLE_COMPLEX = 24;
static const YogiMPI_Datatype YogiMPI_LOGICAL = 25;
static const YogiMPI_Datatype YogiMPI_2REAL = 26;
static const YogiMPI_Datatype YogiMPI_2DOUBLE_PRECISION = 27;
static const YogiMPI_Datatype YogiMPI_2INTEGER = 28;
static const YogiMPI_Datatype YogiMPI_INTEGER1 = 29;
static const YogiMPI_Datatype YogiMPI_INTEGER2 = 30;
static const YogiMPI_Datatype YogiMPI_INTEGER4 = 31;
static const YogiMPI_Datatype YogiMPI_INTEGER8 = 32;
static const YogiMPI_Datatype YogiMPI_REAL4 = 33;
static const YogiMPI_Datatype YogiMPI_REAL8 = 34;

/* Total number of non-volatile datatypes.  Accounts for Fortran ones, too. */
static const int YogiMPI_DATATYPE_VOLATILE_OFFSET = 37;

/* Total number of datatypes, volatile and non-volatile. 
 */
static int num_datatypes = 100; 

/* Now have a pointer to a pool of MPI datatypes, handing references to
 * users without exposing the opaque MPI handle.
 */
static MPI_Datatype *datatype_pool;

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
static int mpi_error_codes[21];

static int num_groups = 100; 
/* Pointer to pool of MPI_Group objects */
static MPI_Group *group_pool = 0;
/* Number of times group pool is reallocated for expansion. */
static int num_realloc_groups = 0; 
/* YogiMPI specific: group underneath YogiMPI_COMM_WORLD */
static const YogiMPI_Group YogiMPI_GROUP_WORLD = 2;
/* YogiMPI specific: group underneath YogiMPI_COMM_SELF */
static const YogiMPI_Group YogiMPI_GROUP_SELF = 3;
/* from this offset onward up to num_groups are volatile in group_pool */
static const YogiMPI_Group YogiMPI_GROUP_VOLATILE_OFFSET = 4; 

/* Pointer to pool of MPI_Comm objects */
static MPI_Comm *comm_pool = 0;
static int num_comms = 100; 
/* Number of times comm pool is reallocated for expansion. */
static int num_realloc_comms = 0;
/* From this offset onward up to num_comms the communicators in comm_pool are
   volatile */
static const YogiMPI_Comm YogiMPI_COMM_VOLATILE_OFFSET = 3;

static int num_requests = 100;
/* Pointer to pool of MPI_Request objects */
static MPI_Request *request_pool = 0;
/* Number of times request pool is reallocated for expansion. */
static int num_realloc_requests = 0;

static int num_ops = 20;
/* Pointer to pool of MPI_Op objects */
static MPI_Op *op_pool = 0;
/* From this offset onward up to num_ops the Op objects in op_pool are
   volatile */
static const YogiMPI_Op YogiMPI_OP_VOLATILE_OFFSET = 13;

/* Do the same thing for MPI_Info and MPI_File pools. */
static int num_files = 100;
static int num_infos = 100;
/* Pointer to pool of MPI_File objects */
static MPI_File *file_pool = 0;
/* Pointer to pool of MPI_Info objects */
static MPI_Info *info_pool = 0;
/* From this offset onward up to num_files the File objects in file_pool are
   volatile */
static const YogiMPI_File YogiMPI_FILE_VOLATILE_OFFSET = 1;
/* From this offset onward up to num_infos the Info objects in info_pool are
   volatile */
static const YogiMPI_Info YogiMPI_INFO_VOLATILE_OFFSET = 1;

/*
 * conversion functions YogiMPI <-> MPI
 */

/* Converts an MPI-error into a YogiMPI-error */
int error_to_yogi(int mpi_error) 
{ 
  int current = 0;
  while(mpi_error_codes[current] != mpi_error) {
    ++current;
    if (current > YogiMPI_ERR_LASTCODE) {
      return YogiMPI_ERR_INTERN;
    }
  }
  return current;
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
void free_datatype_array(MPI_Datatype array_of_types[]) {
    free(array_of_types);
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

static MPI_Aint * aint_array_to_mpi(YogiMPI_Aint in[], int count) {
	int i;
	MPI_Aint * conv_aint = (MPI_Aint *)malloc(count*sizeof(MPI_Aint));
	for (i = 0; i < count; i++) {
		conv_aint[i] = aint_to_mpi(in[i]);
	}
	return conv_aint;	
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
    mpi_error_codes[YogiMPI_ERR_LASTCODE]    = MPI_ERR_LASTCODE;

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

	/* The following will only compile if the Fortran-specific symbols are also 
	 * declared within this file.  These are NOT in the C header. 
	 */
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
        datatype_pool[YogiMPI_LB]                = MPI_LB;
        datatype_pool[YogiMPI_UB]                = MPI_UB;
	
}

static void initialize_group_pool() {
	
	int i;
    assert(!group_pool);
    group_pool = (MPI_Group *)malloc(sizeof(MPI_Group)*num_groups);
    for(i = 0; i < num_groups; ++i) group_pool[i] = MPI_GROUP_NULL;

    register_preindexed_group(YogiMPI_GROUP_NULL, MPI_GROUP_NULL);
    register_preindexed_group(YogiMPI_GROUP_EMPTY, MPI_GROUP_EMPTY);

    MPI_Group group_world;
    MPI_Comm_group(MPI_COMM_WORLD, &group_world);
    register_preindexed_group(YogiMPI_GROUP_WORLD, group_world);

    MPI_Group group_self;
    MPI_Comm_group(MPI_COMM_SELF, &group_self);
    register_preindexed_group(YogiMPI_GROUP_SELF, group_self);	
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

    /* mpich2/test/mpi/pt2pt/bottom.c fails otherwise so there is soth. fishy */
    assert(0 == MPI_BOTTOM);
    assert(YogiMPI_BSEND_OVERHEAD >= MPI_BSEND_OVERHEAD);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Initialize the back-end arrays for opaque objects and references */
    bind_mpi_err_constants();
    if (!datatype_pool) initialize_datatype_pool();
    if (!group_pool) initialize_group_pool();
    if (!comm_pool) initialize_comm_pool();
    if (!request_pool) initialize_request_pool();
    if (!op_pool) initialize_op_pool();
    if (!info_pool) initialize_info_pool();
    if (!file_pool) initialize_file_pool();

}

int YogiMPI_Init(int* argc, char ***argv)
{ 
    int mpi_err = MPI_Init(argc,argv); 
 
    allocate_yogimpi_storage(); 

    return error_to_yogi(mpi_err);
}

int YogiMPI_Send(void* buf, int count, YogiMPI_Datatype datatype, int dest, 
		         int tag, YogiMPI_Comm comm) {
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

int YogiMPI_Ssend(void* buf, int count, YogiMPI_Datatype datatype, int dest, 
		          int tag, YogiMPI_Comm comm) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);

    int mpi_error = MPI_Ssend(buf, count, mpi_datatype, dest, tag, mpi_comm);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Isend(void* buf, int count, YogiMPI_Datatype datatype, int dest, 
		          int tag, YogiMPI_Comm comm, YogiMPI_Request *request) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Request mpi_request;

    int mpi_error = MPI_Isend(buf, count, mpi_datatype, dest, tag, mpi_comm, 
		                      &mpi_request);
    *request = add_new_request(mpi_request);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Issend(void* buf, int count, YogiMPI_Datatype datatype, int dest, 
		           int tag, YogiMPI_Comm comm, YogiMPI_Request *request) {
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
    MPI_Request* mpi_requests = (MPI_Request*)malloc(count*sizeof(MPI_Request));
    for(i = 0; i < count; ++i) {
    	mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }
    int mpi_err;

    if (YogiMPI_STATUSES_IGNORE != array_of_statuses) {
        MPI_Status* mpi_statuses = (MPI_Status*)malloc(count*sizeof(MPI_Status));
        for (i = 0; i < count; i++) {
        	mpi_statuses[i] = *(yogi_status_to_mpi(&array_of_statuses[i]));
        }
        mpi_err = MPI_Waitall(count, mpi_requests, mpi_statuses);
        for(i = 0; i < count; i++) {
        	mpi_status_to_yogi(&mpi_statuses[i], &array_of_statuses[i]);
        }
        free(mpi_statuses);
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

    free(mpi_requests);

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

int YogiMPI_Send_init(void *buf, int count, YogiMPI_Datatype datatype, int dest,
		              int tag, YogiMPI_Comm comm, YogiMPI_Request* request) {
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
    int mpi_err = MPI_Bcast(buffer, count, mpi_datatype, root, mpi_comm);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Gather(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype, 
		           void* recvbuf, int recvcount, YogiMPI_Datatype recvtype, 
				   int root, YogiMPI_Comm comm) {

    int mpi_err = YogiMPI_SUCCESS ;
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Gather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, recvbuf,
        		             recvcount, mpi_recvtype, root, mpi_comm);
    } 
    else { 
        MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
        mpi_err = MPI_Gather(sendbuf, sendcount, mpi_sendtype, recvbuf,
        		             recvcount, mpi_recvtype, root, mpi_comm);
    }
    return error_to_yogi(mpi_err);
}

int YogiMPI_Gatherv(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype, 
		            void* recvbuf, int *recvcounts, int *displs, 
					YogiMPI_Datatype recvtype, int root, YogiMPI_Comm comm) {
    int mpi_err = YogiMPI_SUCCESS ;
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Gatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, recvbuf,
        		              recvcounts, displs, mpi_recvtype, root, mpi_comm);
    } 
    else { 
        MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
        mpi_err = MPI_Gatherv(sendbuf, sendcount, mpi_sendtype, recvbuf,
        		              recvcounts, displs, mpi_recvtype, root, mpi_comm);
    }

    return error_to_yogi(mpi_err);
}

int YogiMPI_Scatter(void *sendbuf, int sendcount, YogiMPI_Datatype sendtype,
                    void *recvbuf, int recvcount, YogiMPI_Datatype recvtype, 
					int root, YogiMPI_Comm comm) {
	int mpi_err = YogiMPI_SUCCESS;
	MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
	MPI_Comm mpi_comm = comm_to_mpi(comm);
	if (YogiMPI_IN_PLACE == sendbuf) {
	    mpi_err = MPI_Scatter(sendbuf, sendcount, mpi_sendtype, MPI_IN_PLACE,
	    		              0, MPI_DATATYPE_NULL, root, mpi_comm);
	} 
	else { 
	    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
	    mpi_err = MPI_Scatter(sendbuf, sendcount, mpi_sendtype, recvbuf,
	    		              recvcount, mpi_recvtype, root, mpi_comm);
	}
	return error_to_yogi(mpi_err);
	
}

int YogiMPI_Scatterv(void* sendbuf, int *sendcounts, int *displs, 
		             YogiMPI_Datatype sendtype, void* recvbuf, int recvcount,
					 YogiMPI_Datatype recvtype, int root, YogiMPI_Comm comm) {
  int mpi_err = YogiMPI_SUCCESS;
  MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
  MPI_Comm mpi_comm = comm_to_mpi(comm);
  if (YogiMPI_IN_PLACE == sendbuf) {
      mpi_err = MPI_Scatterv(sendbuf, sendcounts, displs, mpi_sendtype,
    		                 MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, root,
							 mpi_comm);
  } 
  else { 
      MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype) ;
      mpi_err = MPI_Scatterv(sendbuf, sendcounts, displs, mpi_sendtype, recvbuf,
    		                 recvcount, mpi_recvtype, root, mpi_comm);
  }
  return error_to_yogi(mpi_err);
}

int YogiMPI_Allgather(void* sendbuf, int sendcount, YogiMPI_Datatype sendtype, 
		              void* recvbuf, int recvcount, YogiMPI_Datatype recvtype, 
					  YogiMPI_Comm comm) {
    int mpi_err = YogiMPI_SUCCESS;
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    if (YogiMPI_IN_PLACE == sendbuf) {
        mpi_err = MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, recvbuf,
        		                recvcount, mpi_recvtype, mpi_comm);
    } 
    else {
        MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
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
      mpi_err = MPI_Reduce(MPI_IN_PLACE, recvbuf, count, mpi_datatype, mpi_op,
    		               root, mpi_comm);
  } 
  else {
      mpi_err = MPI_Reduce(sendbuf, recvbuf, count, mpi_datatype, mpi_op, root,
    		               mpi_comm);
  }

  return error_to_yogi(mpi_err);
  
}

int YogiMPI_Allreduce(void* sendbuf, void* recvbuf, int count, 
		              YogiMPI_Datatype datatype, YogiMPI_Op op, 
					  YogiMPI_Comm comm) {
    int mpi_err = YogiMPI_SUCCESS ;
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
    if (request_pool) {
        free(request_pool);
        request_pool = 0;
    }
    if (comm_pool) { 
    	free(comm_pool);
    	comm_pool = 0;
    }
    if (group_pool) {
        free(group_pool);
        group_pool = 0;
    }
    if (info_pool) {
        free(info_pool);
        info_pool = 0;
    }
    if (file_pool) {
        free(file_pool);
        file_pool = 0;
    }
    int mpi_err = MPI_Finalize();
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
    int mpi_error = MPI_File_open(mpi_comm, filename, amode, mpi_info, 
    		                      &mpi_file);
    *fh = add_new_file(mpi_file);
    
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
		                  const void *buf, int count, YogiMPI_Datatype datatype, 
					      YogiMPI_Status *status) {
	
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

int YogiMPI_Info_get(YogiMPI_Info info, char* key, int valuelen, char* value, int* flag) {

    MPI_Info conv_info = info_to_mpi(info);
    int mpi_error;
    mpi_error = MPI_Info_get(conv_info, key, valuelen, value, flag);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Group_excl(YogiMPI_Group group, int n, int* ranks, YogiMPI_Group* newgroup) {

    MPI_Group conv_group = group_to_mpi(group);
    MPI_Group conv_newgroup;
    int mpi_error;
    mpi_error = MPI_Group_excl(conv_group, n, ranks, &conv_newgroup);
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
