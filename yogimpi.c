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
static const int YogiMPI_DATATYPE_VOLATILE_OFFSET = 35;

/* Total number of datatypes, volatile and non-volatile.
 * Set the initial number of datatypes to the total non-volatile amount. 
 */
static int num_datatypes = 35; 

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
static YogiMPI_Datatype alloc_new_volatile_datatype(MPI_Datatype mpi_datatype)
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

static int num_groups = 10; 
/* Pointer to pool of MPI_Group objects */
static MPI_Group *group_pool = 0;
/* Number of times group pool is reallocated for expansion. */
static int num_realloc_groups = 0; 
/* number of references to the group */
static int *group_ref_counts = 0; 
/* YogiMPI specific: group underneath YogiMPI_COMM_WORLD */
static const YogiMPI_Group YogiMPI_GROUP_WORLD = 2;
/* YogiMPI specific: group underneath YogiMPI_COMM_SELF */
static const YogiMPI_Group YogiMPI_GROUP_SELF = 3;
/* from this offset onward up to num_groups are volatile in group_pool */
static const YogiMPI_Group YogiMPI_GROUP_VOLATILE_OFFSET = 4; 

/* Pointer to pool of MPI_Comm objects */
static MPI_Comm *comm_pool = 0;
static int num_comms = 10; 
/* Number of times comm pool is reallocated for expansion. */
static int num_realloc_comms = 0;
/* From this offset onward up to num_comms the communicators in comm_pool are
   volatile */
static const YogiMPI_Comm YogiMPI_COMM_VOLATILE_OFFSET = 3; 
static char* is_inter_comms;

/* Mapping between YogiMPI comms and YogiMPI groups */
static YogiMPI_Group *comm_groups_map; 

static int num_requests = 10;
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
      return YogiMPI_ERR_INTERN ;
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

/* Convert MPI comparison constants to YogiMPI comparison constants */
int comparison_to_yogi(int mpi_comp)
{
  if (mpi_comp == MPI_IDENT) return YogiMPI_IDENT;
  if (mpi_comp == MPI_CONGRUENT) return YogiMPI_CONGRUENT;
  if (mpi_comp == MPI_SIMILAR) return YogiMPI_SIMILAR;
  if (mpi_comp == MPI_UNEQUAL) return YogiMPI_UNEQUAL;
  return YogiMPI_UNEQUAL;
}

/* Convert an MPI_Status object into a YogiMPI_Status object */
void status_to_yogi(MPI_Status* mpi_status, YogiMPI_Status* status)
{
  assert(status != YogiMPI_STATUS_IGNORE);
  /* Reassign basic integer tags so they can be read by user */
  status->YogiMPI_SOURCE = mpi_status->MPI_SOURCE; 
  status->YogiMPI_TAG = mpi_status->MPI_TAG;
  status->YogiMPI_ERROR = mpi_status->MPI_ERROR;
  /* Pack the actual status object as a void pointer, which can be used
   * if/when pushing the object from YogiMPI back to MPI-land
   */
  status->mpi_status = (void *)mpi_status;
}

/* Register pre-indexed group */
static void register_preindexed_group(YogiMPI_Group group, MPI_Group mpi_group) 
{
  assert(0 == group_ref_counts[group]);
  assert(group >= 0 && group < YogiMPI_GROUP_VOLATILE_OFFSET);
  group_pool[group] = mpi_group;
  group_ref_counts[group] = 1;
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
static YogiMPI_Group alloc_new_volatile_group(MPI_Group mpi_group)
{

    /* find new slot */
    YogiMPI_Group new_slot = YogiMPI_GROUP_VOLATILE_OFFSET ;
    for(; group_pool[new_slot] != MPI_GROUP_NULL && 
          new_slot < num_groups; ++new_slot);

    /* if not sufficient slots, increase number of slots */
    if (new_slot == num_groups) {
        int new_num_groups = num_groups * 2;
        int i;

        ++num_realloc_groups; /* update stats */

        /* realloc group_pool and group_ref_counts */
        MPI_Group* new_groups = (MPI_Group*)malloc(new_num_groups*sizeof(MPI_Group));
        memcpy(new_groups,group_pool,num_groups*sizeof(MPI_Group)) ;
        free(group_pool);

        int *new_group_ref_counts = (int*)malloc(new_num_groups*sizeof(int));
        memcpy(new_group_ref_counts,group_ref_counts,num_groups*sizeof(int));
        free(group_ref_counts);

        for(i = num_groups; i < new_num_groups; ++i) {
            new_groups[i] = MPI_GROUP_NULL;
            new_group_ref_counts[i] = 0;
        }

        group_pool = new_groups;
        group_ref_counts = new_group_ref_counts;
        num_groups = new_num_groups;
    }

    assert(new_slot < num_groups);
    assert(group_pool[new_slot] == MPI_GROUP_NULL &&
		   group_ref_counts[new_slot] == 0);
    group_pool[new_slot] = mpi_group;
    group_ref_counts[new_slot] = 1;

    return new_slot;
}

static void register_preindexed_comm(YogiMPI_Comm comm, MPI_Comm mpi_comm, 
		                             YogiMPI_Group group) {
    assert(comm >= 0 && comm < num_comms);
    assert(group >= 0 && group < num_groups);
    assert(MPI_COMM_NULL == comm_pool[comm]);
    assert(YogiMPI_GROUP_NULL == comm_groups_map[comm]) ;

    comm_pool[comm] = mpi_comm;
    comm_groups_map[comm] = group;
}

YogiMPI_Comm alloc_new_volatile_comm(MPI_Comm mpi_comm, YogiMPI_Group group, 
		                             int is_inter) {

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

        YogiMPI_Group* new_comm_groups = (YogiMPI_Group*)malloc(new_num_comms*sizeof(YogiMPI_Group));
        memcpy(new_comm_groups,comm_groups_map,num_comms*sizeof(YogiMPI_Group));
        free(comm_groups_map);

        char* new_is_inter_comms = (char*)malloc(new_num_comms*sizeof(char));
        memcpy(new_is_inter_comms,is_inter_comms,num_comms*sizeof(char));
        free(is_inter_comms);

        for(i = num_comms; i < new_num_comms; ++i) new_comms[i] = MPI_COMM_NULL;
        for(i = num_comms; i < new_num_comms; ++i) new_comm_groups[i] = YogiMPI_GROUP_NULL;
        for(i = num_comms; i < new_num_comms; ++i) new_is_inter_comms[i] = 'n';

        comm_pool = new_comms;
        comm_groups_map = new_comm_groups;
        is_inter_comms = new_is_inter_comms;
        num_comms = new_num_comms;
    }

    assert(comm_pool[new_slot] == MPI_COMM_NULL && 
    	   comm_groups_map[new_slot] == YogiMPI_GROUP_NULL);
    comm_pool[new_slot] = mpi_comm;
    comm_groups_map[new_slot] = group;
    is_inter_comms[new_slot] = is_inter ? 'y' : 'n';
    group_ref_counts[group] += 1;

    return new_slot;
}

YogiMPI_Request alloc_new_request(MPI_Request mpi_request)
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

YogiMPI_Comm alloc_new_comm_and_group(MPI_Comm mpi_comm, MPI_Group mpi_group) {
    YogiMPI_Group group = alloc_new_volatile_group(mpi_group);
    YogiMPI_Comm new_comm = alloc_new_volatile_comm(mpi_comm, group, 0);

    /* Both alloc's above will increase the ref-count of the group and thus set 
     * it at 2 although the user has only one reference (through the 
     * communicator).
     */
    assert(2 == group_ref_counts[group]);
    --group_ref_counts[group];

    return new_comm;
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
	
}

static void initialize_group_pool() {
	
	int i;
    assert(!group_pool);
    group_pool = (MPI_Group *)malloc(sizeof(MPI_Group)*num_groups);
    for(i = 0; i < num_groups; ++i) group_pool[i] = MPI_GROUP_NULL;
    
    group_ref_counts = (int *)malloc(num_groups*sizeof(int));
    for(i = 0; i < num_groups; ++i) group_ref_counts[i] = 0;

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
    
    for(i = 0; i < num_comms; ++i) comm_pool[i] = MPI_COMM_NULL;
    comm_groups_map = (YogiMPI_Group *)malloc(num_comms*sizeof(YogiMPI_Group));
    
    for(i = 0; i < num_comms; ++i) comm_groups_map[i] = YogiMPI_GROUP_NULL;
    is_inter_comms = (char*)malloc(num_comms*sizeof(char));
    
    for(i = 0; i < num_comms; ++i) is_inter_comms[i] = 'n';

    register_preindexed_comm(YogiMPI_COMM_NULL, MPI_COMM_NULL, 
    		                 YogiMPI_GROUP_NULL);
    register_preindexed_comm(YogiMPI_COMM_WORLD, MPI_COMM_WORLD,
    		                 YogiMPI_GROUP_WORLD);
    register_preindexed_comm(YogiMPI_COMM_SELF, MPI_COMM_SELF,
    		                 YogiMPI_GROUP_SELF);	
}

static void initialize_request_pool() {
	int i;
    request_pool = (MPI_Request *)malloc(num_requests*sizeof(MPI_Request));
    for(i = 0; i < num_requests; ++i) request_pool[i] = MPI_REQUEST_NULL;	
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

int YogiMPI_Init(int* argc, char ***argv)
{ 
    int mpi_err = MPI_Init(argc,argv); 
  
    /* mpich2/test/mpi/pt2pt/bottom.c fails otherwise so there is soth. fishy */
    assert(0 == MPI_BOTTOM);
    assert(YogiMPI_BSEND_OVERHEAD >= MPI_BSEND_OVERHEAD);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Initialize the back-end arrays for opaque objects and references */
    bind_mpi_err_constants();
    initialize_datatype_pool();
    initialize_group_pool();
    initialize_comm_pool();
    initialize_request_pool();
    initialize_op_pool();

    /* Verify MPI_Status definition is the size expected by YogiMPI.  If not,
     * a lot of code will fail to work as expected.
     */
    assert(sizeof(YogiMPI_Status) == 6 * sizeof(int));

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
        MPI_Status* mpi_status;
        mpi_status = (MPI_Status*)malloc(sizeof(MPI_Status));
        mpi_error = MPI_Recv(buf, count, mpi_datatype, source, tag, mpi_comm, 
        		             mpi_status);
        status_to_yogi(mpi_status, status); 
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
    int mpi_err = MPI_Get_count((MPI_Status*)status->mpi_status, mpi_datatype,
    		                    count);
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
    *request = alloc_new_request(mpi_request);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Issend(void* buf, int count, YogiMPI_Datatype datatype, int dest, 
		           int tag, YogiMPI_Comm comm, YogiMPI_Request *request) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Request mpi_request;

    int mpi_error = MPI_Issend(buf, count, mpi_datatype, dest, tag, mpi_comm, 
    		                   &mpi_request);
    *request = alloc_new_request(mpi_request);

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
    *request = alloc_new_request(mpi_request);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Wait(YogiMPI_Request* request, YogiMPI_Status* status) {
	
    MPI_Request* mpi_request = request_to_mpi(*request);
    int mpi_err;
    if (YogiMPI_STATUS_IGNORE != status) {
        MPI_Status mpi_status;
        mpi_err = MPI_Wait(mpi_request, &mpi_status);
        status_to_yogi(&mpi_status, status);
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
        mpi_err = MPI_Waitall(count, mpi_requests, mpi_statuses);
        for(i = 0; i < count; ++i) status_to_yogi(mpi_statuses + i,
        		                                  array_of_statuses + i);
        free(mpi_statuses);
    }
    else {
        mpi_err = MPI_Waitall(count, mpi_requests, MPI_STATUSES_IGNORE);
    }
 
    /* reset requests */
    for(i = 0; i < count; ++i) {
        assert(MPI_REQUEST_NULL == mpi_requests[i]);
        * request_to_mpi(array_of_requests[i]) = MPI_REQUEST_NULL; 
        array_of_requests[i] = YogiMPI_REQUEST_NULL;
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
    *request = alloc_new_request(mpi_request);

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

    *newtype = alloc_new_volatile_datatype(mpi_newtype);

    return error_to_yogi(mpi_err);
}

int YogiMPI_Type_vector(int count, int blocklength, int stride, 
		                YogiMPI_Datatype oldtype, YogiMPI_Datatype *newtype) {
    MPI_Datatype mpi_oldtype = datatype_to_mpi(oldtype);
    MPI_Datatype mpi_newtype = MPI_DATATYPE_NULL;

    int mpi_err = MPI_Type_vector(count, blocklength, stride, mpi_oldtype, 
    		                      &mpi_newtype);
    *newtype = alloc_new_volatile_datatype(mpi_newtype);
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
    *newtype = alloc_new_volatile_datatype(mpi_newtype);
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

  *out = alloc_new_volatile_comm(mpi_out, comm_groups_map[comm], 0);

  return error_to_yogi(mpi_err);
}

int YogiMPI_Comm_split(YogiMPI_Comm comm, int color, int key, 
		               YogiMPI_Comm* newcomm) {
    
	MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Comm mpi_newcomm = MPI_COMM_NULL;

    if ( YogiMPI_UNDEFINED == color ) {
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

        /* The creation of the comm induces the creation of a new group 
         * underneath */
        MPI_Group mpi_newgroup = MPI_GROUP_NULL;
        MPI_Comm_group(mpi_newcomm, &mpi_newgroup);
        *newcomm = alloc_new_comm_and_group(mpi_newcomm, mpi_newgroup);
        MPI_Group_free(&mpi_newgroup);
        return error_to_yogi(mpi_err);
    }
}

int YogiMPI_Comm_free(YogiMPI_Comm *comm) {
    assert(*comm < YogiMPI_COMM_VOLATILE_OFFSET || *comm >= num_comms);
    assert(comm_pool[*comm] != MPI_COMM_NULL);

    MPI_Comm* mpi_comm = &comm_pool[*comm];
    MPI_Comm_free(mpi_comm);

    YogiMPI_Group group = comm_groups_map[*comm];
    assert(group_ref_counts[group] > 0);
    --group_ref_counts[group];
    if (group_ref_counts[group] == 0) group_pool[group] = MPI_GROUP_NULL;
    comm_groups_map[*comm] = YogiMPI_GROUP_NULL;
    comm_pool[*comm] = MPI_COMM_NULL;
    is_inter_comms[*comm] = 'n';

    *comm = YogiMPI_COMM_NULL;

    return MPI_SUCCESS;
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
    if (comm_groups_map) {
        free(comm_groups_map); 
        comm_groups_map = 0;
    }
    if (comm_pool) { 
    	free(comm_pool);
    	comm_pool = 0;
    }
    if (group_ref_counts) {
        free(group_ref_counts);
        group_ref_counts = 0;
    }
    if (group_pool) {
        free(group_pool);
        group_pool = 0;
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
    *newtype = alloc_new_volatile_datatype(mpi_newtype);
    return error_to_yogi(mpi_err);
}

YogiMPI_Comm YogiMPI_Comm_f2c(YogiMPI_Fint comm) {
    return comm;	
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
    *request = alloc_new_request(mpi_request);

    return error_to_yogi(mpi_error);
}

int YogiMPI_Scan(const void *sendbuf, void *recvbuf, int count, 
		         YogiMPI_Datatype datatype, YogiMPI_Op op, YogiMPI_Comm comm) {

    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    MPI_Op mpi_op op_to_mpi(op);

    int mpi_error = MPI_Scan(sendbuf, recvbuf, count, mpi_datatype, mpi_op,
    		                 mpi_comm);
    
    return error_to_yogi(mpi_error);	
}

int YogiMPI_Startall(int count, YogiMPI_Request *array_of_requests) {
    int i;
    MPI_Request* mpi_requests = (MPI_Request*)malloc(count*sizeof(MPI_Request));
    for(i = 0; i < count; ++i) {
    	mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }
    
	int mpi_error = MPI_Startall(count, mpi_requests)
			
    /* reset requests */
    for(i = 0; i < count; ++i) {
        assert(MPI_REQUEST_NULL == mpi_requests[i]);
    	* request_to_mpi(array_of_requests[i]) = MPI_REQUEST_NULL; 
    	array_of_requests[i] = YogiMPI_REQUEST_NULL;
    }

    free(mpi_requests);
    
    return error_to_yogi(mpi_error);	
}

int YogiMPI_Alltoall(const void *sendbuf, int sendcount, 
		             YogiMPI_Datatype sendtype, void *recvbuf, int recvcount,
					 YogiMPI_Datatype recvtype, YogiMPI_Comm comm) {
    MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);

    int mpi_error = MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf,
    		                     recvcount, recvtype, mpi_comm);
    
    return error_to_yogi(mpi_error);	

}

int YogiMPI_Alltoallv(const void *sendbuf, const int *sendcounts,
                      const int *sdispls, YogiMPI_Datatype sendtype, 
					  void *recvbuf, const int *recvcounts, const int *rdispls,
					  YogiMPI_Datatype recvtype, YogiMPI_Comm comm) {

    MPI_Datatype mpi_sendtype = datatype_to_mpi(sendtype);
    MPI_Datatype mpi_recvtype = datatype_to_mpi(recvtype);
    MPI_Comm mpi_comm = comm_to_mpi(comm);

    int mpi_error = MPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, 
    		                      recvbuf, recvcounts, rdispls, recvtype, 
								  mpi_comm);
    
    return error_to_yogi(mpi_error);	

}
