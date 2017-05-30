#include "YogiManager.h"
#include <algorithm>
#include <iterator>
#include <cstring>
#ifdef YOGI_DEBUG
#include <iostream>
#endif

const int YogiManager::defaultPoolSize = 100;

YogiManager* YogiManager::_instance = 0;

YogiManager* YogiManager::getInstance() {
    if (_instance == 0) {
        _instance = new YogiManager;
    }
    return _instance;
}

YogiManager::YogiManager() {
    errPool.resize(defaultPoolSize, MPI_ERRHANDLER_NULL);
    numErrs = errOffset = 3;
    commPool.resize(defaultPoolSize, MPI_COMM_NULL);
    numComms = commOffset = 3;
    requestPool.resize(defaultPoolSize, MPI_REQUEST_NULL);
    numRequests = requestOffset = 1;
    winPool.resize(defaultPoolSize, MPI_WIN_NULL);
    numWins = winOffset = 1;
    opPool.resize(defaultPoolSize, MPI_OP_NULL);
    numOps = opOffset = 13;
    datatypePool.resize(defaultPoolSize, MPI_DATATYPE_NULL);
    numDatatypes = datatypeOffset = 38;
    infoPool.resize(defaultPoolSize, MPI_INFO_NULL);
    numInfos = infoOffset = 1;
    groupPool.resize(defaultPoolSize, MPI_GROUP_NULL);
    numGroups = groupOffset = 2;
    filePool.resize(defaultPoolSize, MPI_FILE_NULL);
    numFiles = fileOffset = 1;

    mpiErrors[YogiMPI_SUCCESS]         = MPI_SUCCESS;
    mpiErrors[YogiMPI_ERR_BUFFER]      = MPI_ERR_BUFFER;
    mpiErrors[YogiMPI_ERR_COUNT]       = MPI_ERR_COUNT;
    mpiErrors[YogiMPI_ERR_TYPE]        = MPI_ERR_TYPE;
    mpiErrors[YogiMPI_ERR_TAG]         = MPI_ERR_TAG;
    mpiErrors[YogiMPI_ERR_COMM]        = MPI_ERR_COMM;
    mpiErrors[YogiMPI_ERR_RANK]        = MPI_ERR_RANK;
    mpiErrors[YogiMPI_ERR_REQUEST]     = MPI_ERR_REQUEST;
    mpiErrors[YogiMPI_ERR_ROOT]        = MPI_ERR_ROOT;
    mpiErrors[YogiMPI_ERR_GROUP]       = MPI_ERR_GROUP;
    mpiErrors[YogiMPI_ERR_OP]          = MPI_ERR_OP;
    mpiErrors[YogiMPI_ERR_TOPOLOGY]    = MPI_ERR_TOPOLOGY;
    mpiErrors[YogiMPI_ERR_DIMS]        = MPI_ERR_DIMS;
    mpiErrors[YogiMPI_ERR_ARG]         = MPI_ERR_ARG;
    mpiErrors[YogiMPI_ERR_UNKNOWN]     = MPI_ERR_UNKNOWN;
    mpiErrors[YogiMPI_ERR_TRUNCATE]    = MPI_ERR_TRUNCATE;
    mpiErrors[YogiMPI_ERR_OTHER]       = MPI_ERR_OTHER;
    mpiErrors[YogiMPI_ERR_INTERN]      = MPI_ERR_INTERN;
    mpiErrors[YogiMPI_ERR_PENDING]     = MPI_ERR_PENDING;
    mpiErrors[YogiMPI_ERR_IN_STATUS]   = MPI_ERR_IN_STATUS;
    mpiErrors[YogiMPI_ERR_FILE]        = MPI_ERR_FILE;
    mpiErrors[YogiMPI_ERR_NOT_SAME]    = MPI_ERR_NOT_SAME;
    mpiErrors[YogiMPI_ERR_AMODE]       = MPI_ERR_AMODE;
    mpiErrors[YogiMPI_ERR_UNSUPPORTED_DATAREP]
                                        = MPI_ERR_UNSUPPORTED_DATAREP;
    mpiErrors[YogiMPI_ERR_UNSUPPORTED_OPERATION]
                                        = MPI_ERR_UNSUPPORTED_OPERATION;
    mpiErrors[YogiMPI_ERR_NO_SUCH_FILE] = MPI_ERR_NO_SUCH_FILE;
    mpiErrors[YogiMPI_ERR_FILE_EXISTS]  = MPI_ERR_FILE_EXISTS;
    mpiErrors[YogiMPI_ERR_BAD_FILE]     = MPI_ERR_BAD_FILE;
    mpiErrors[YogiMPI_ERR_ACCESS]       = MPI_ERR_ACCESS;
    mpiErrors[YogiMPI_ERR_NO_SPACE]     = MPI_ERR_NO_SPACE;
    mpiErrors[YogiMPI_ERR_QUOTA]        = MPI_ERR_QUOTA;
    mpiErrors[YogiMPI_ERR_READ_ONLY]    = MPI_ERR_READ_ONLY;
    mpiErrors[YogiMPI_ERR_FILE_IN_USE]  = MPI_ERR_FILE_IN_USE;
    mpiErrors[YogiMPI_ERR_DUP_DATAREP]  = MPI_ERR_DUP_DATAREP;
    mpiErrors[YogiMPI_ERR_CONVERSION]   = MPI_ERR_CONVERSION;
    mpiErrors[YogiMPI_ERR_IO]           = MPI_ERR_IO;
    mpiErrors[YogiMPI_ERR_LASTCODE]     = MPI_ERR_LASTCODE;

    yogiErrors[MPI_SUCCESS]         = YogiMPI_SUCCESS;
    yogiErrors[MPI_ERR_BUFFER]      = YogiMPI_ERR_BUFFER;
    yogiErrors[MPI_ERR_COUNT]       = YogiMPI_ERR_COUNT;
    yogiErrors[MPI_ERR_TYPE]        = YogiMPI_ERR_TYPE;
    yogiErrors[MPI_ERR_TAG]         = YogiMPI_ERR_TAG;
    yogiErrors[MPI_ERR_COMM]        = YogiMPI_ERR_COMM;
    yogiErrors[MPI_ERR_RANK]        = YogiMPI_ERR_RANK;
    yogiErrors[MPI_ERR_REQUEST]     = YogiMPI_ERR_REQUEST;
    yogiErrors[MPI_ERR_ROOT]        = YogiMPI_ERR_ROOT;
    yogiErrors[MPI_ERR_GROUP]       = YogiMPI_ERR_GROUP;
    yogiErrors[MPI_ERR_OP]          = YogiMPI_ERR_OP;
    yogiErrors[MPI_ERR_TOPOLOGY]    = YogiMPI_ERR_TOPOLOGY;
    yogiErrors[MPI_ERR_DIMS]        = YogiMPI_ERR_DIMS;
    yogiErrors[MPI_ERR_ARG]         = YogiMPI_ERR_ARG;
    yogiErrors[MPI_ERR_UNKNOWN]     = YogiMPI_ERR_UNKNOWN;
    yogiErrors[MPI_ERR_TRUNCATE]    = YogiMPI_ERR_TRUNCATE;
    yogiErrors[MPI_ERR_OTHER]       = YogiMPI_ERR_OTHER;
    yogiErrors[MPI_ERR_INTERN]      = YogiMPI_ERR_INTERN;
    yogiErrors[MPI_ERR_PENDING]     = YogiMPI_ERR_PENDING;
    yogiErrors[MPI_ERR_IN_STATUS]   = YogiMPI_ERR_IN_STATUS;
    yogiErrors[MPI_ERR_FILE]        = YogiMPI_ERR_FILE;
    yogiErrors[MPI_ERR_NOT_SAME]    = YogiMPI_ERR_NOT_SAME;
    yogiErrors[MPI_ERR_AMODE]       = YogiMPI_ERR_AMODE;
    yogiErrors[MPI_ERR_UNSUPPORTED_DATAREP]
                                       = YogiMPI_ERR_UNSUPPORTED_DATAREP;
    yogiErrors[MPI_ERR_UNSUPPORTED_OPERATION]
                                         = YogiMPI_ERR_UNSUPPORTED_OPERATION;
    yogiErrors[MPI_ERR_NO_SUCH_FILE] = YogiMPI_ERR_NO_SUCH_FILE;
    yogiErrors[MPI_ERR_FILE_EXISTS]  = YogiMPI_ERR_FILE_EXISTS;
    yogiErrors[MPI_ERR_BAD_FILE]     = YogiMPI_ERR_BAD_FILE;
    yogiErrors[MPI_ERR_ACCESS]       = YogiMPI_ERR_ACCESS;
    yogiErrors[MPI_ERR_NO_SPACE]     = YogiMPI_ERR_NO_SPACE;
    yogiErrors[MPI_ERR_QUOTA]        = YogiMPI_ERR_QUOTA;
    yogiErrors[MPI_ERR_READ_ONLY]    = YogiMPI_ERR_READ_ONLY;
    yogiErrors[MPI_ERR_FILE_IN_USE]  = YogiMPI_ERR_FILE_IN_USE;
    yogiErrors[MPI_ERR_DUP_DATAREP]  = YogiMPI_ERR_DUP_DATAREP;
    yogiErrors[MPI_ERR_CONVERSION]   = YogiMPI_ERR_CONVERSION;
    yogiErrors[MPI_ERR_IO]           = YogiMPI_ERR_IO;
    yogiErrors[MPI_ERR_LASTCODE]     = YogiMPI_ERR_LASTCODE;

    yogiComps[MPI_IDENT] = YogiMPI_IDENT; 
    yogiComps[MPI_CONGRUENT] = YogiMPI_CONGRUENT;
    yogiComps[MPI_SIMILAR] = YogiMPI_SIMILAR;
    yogiComps[MPI_UNEQUAL] = YogiMPI_UNEQUAL;

    // Set group pool constants
    groupPool.at(YogiMPI_GROUP_EMPTY) = MPI_GROUP_EMPTY;

    // Set comm pool constants
    commPool.at(YogiMPI_COMM_WORLD) = MPI_COMM_WORLD;
    commPool.at(YogiMPI_COMM_SELF) = MPI_COMM_SELF;

    // Set op pool constants
    opPool.at(YogiMPI_MAX)    = MPI_MAX;
    opPool.at(YogiMPI_MIN)    = MPI_MIN;
    opPool.at(YogiMPI_SUM)    = MPI_SUM;
    opPool.at(YogiMPI_PROD)   = MPI_PROD;
    opPool.at(YogiMPI_MAXLOC) = MPI_MAXLOC;
    opPool.at(YogiMPI_MINLOC) = MPI_MINLOC;
    opPool.at(YogiMPI_BAND)   = MPI_BAND;
    opPool.at(YogiMPI_BOR)    = MPI_BOR;
    opPool.at(YogiMPI_BXOR)   = MPI_BXOR;
    opPool.at(YogiMPI_LAND)   = MPI_LAND;
    opPool.at(YogiMPI_LOR)    = MPI_LOR;
    opPool.at(YogiMPI_LXOR)   = MPI_LXOR;

    // Set errhandler pool constants
    errPool.at(YogiMPI_ERRORS_ARE_FATAL) = MPI_ERRORS_ARE_FATAL;
    errPool.at(YogiMPI_ERRORS_RETURN)    = MPI_ERRORS_RETURN;

    // Set datatype pool constants
    datatypePool.at(YogiMPI_CHAR)              = MPI_CHAR;
    datatypePool.at(YogiMPI_SHORT)             = MPI_SHORT;
    datatypePool.at(YogiMPI_INT)               = MPI_INT;
    datatypePool.at(YogiMPI_LONG)              = MPI_LONG;
    datatypePool.at(YogiMPI_UNSIGNED_CHAR)     = MPI_UNSIGNED_CHAR;
    datatypePool.at(YogiMPI_UNSIGNED_SHORT)    = MPI_UNSIGNED_SHORT;
    datatypePool.at(YogiMPI_UNSIGNED)          = MPI_UNSIGNED;
    datatypePool.at(YogiMPI_UNSIGNED_LONG)     = MPI_UNSIGNED_LONG;
    datatypePool.at(YogiMPI_FLOAT)             = MPI_FLOAT;
    datatypePool.at(YogiMPI_DOUBLE)            = MPI_DOUBLE;
    datatypePool.at(YogiMPI_LONG_DOUBLE)       = MPI_LONG_DOUBLE;
    datatypePool.at(YogiMPI_BYTE)              = MPI_BYTE;
    datatypePool.at(YogiMPI_PACKED)            = MPI_PACKED;
    datatypePool.at(YogiMPI_FLOAT_INT)         = MPI_FLOAT_INT;
    datatypePool.at(YogiMPI_DOUBLE_INT)        = MPI_DOUBLE_INT;
    datatypePool.at(YogiMPI_LONG_INT)          = MPI_LONG_INT;
    datatypePool.at(YogiMPI_2INT)              = MPI_2INT;
    datatypePool.at(YogiMPI_SHORT_INT)         = MPI_SHORT_INT;
    datatypePool.at(YogiMPI_LONG_DOUBLE_INT)   = MPI_LONG_DOUBLE_INT;
    datatypePool.at(YogiMPI_LONG_LONG_INT)     = MPI_LONG_LONG_INT;
    datatypePool.at(YogiMPI_INT32_T)           = MPI_INT32_T;
    datatypePool.at(YogiMPI_INT64_T)           = MPI_INT64_T;
    datatypePool.at(YogiMPI_COMPLEX)           = MPI_COMPLEX;
    datatypePool.at(YogiMPI_DOUBLE_COMPLEX)    = MPI_DOUBLE_COMPLEX;
    datatypePool.at(YogiMPI_LOGICAL)           = MPI_LOGICAL;
    datatypePool.at(YogiMPI_2REAL)             = MPI_2REAL;
    datatypePool.at(YogiMPI_2DOUBLE_PRECISION) = MPI_2DOUBLE_PRECISION;
    datatypePool.at(YogiMPI_2INTEGER)          = MPI_2INTEGER;
    datatypePool.at(YogiMPI_INTEGER1)          = MPI_INTEGER1;
    datatypePool.at(YogiMPI_INTEGER2)          = MPI_INTEGER2;
    datatypePool.at(YogiMPI_INTEGER4)          = MPI_INTEGER4;
    datatypePool.at(YogiMPI_INTEGER8)          = MPI_INTEGER8;
    datatypePool.at(YogiMPI_REAL4)             = MPI_REAL4;
    datatypePool.at(YogiMPI_REAL8)             = MPI_REAL8;
    datatypePool.at(YogiMPI_UNSIGNED_LONG_LONG) = MPI_UNSIGNED_LONG_LONG;
    datatypePool.at(YogiMPI_LB)                = MPI_LB;
    datatypePool.at(YogiMPI_UB)                = MPI_UB;
    datatypePool.at(YogiMPI_SIGNED_CHAR)       = MPI_SIGNED_CHAR;

}

int YogiManager::combinerToYogi(int in_combiner) {
    switch (in_combiner) {
        case MPI_COMBINER_NAMED:
            return YogiMPI_COMBINER_NAMED;
            break;
        case MPI_COMBINER_DUP:
    	    return YogiMPI_COMBINER_DUP;        
            break;
        case MPI_COMBINER_CONTIGUOUS:
    	    return YogiMPI_COMBINER_CONTIGUOUS;        
            break;
        case MPI_COMBINER_VECTOR:
    	    return YogiMPI_COMBINER_VECTOR;
            break;
        case MPI_COMBINER_HVECTOR_INTEGER:
    	    return YogiMPI_COMBINER_HVECTOR_INTEGER;
            break;
        case MPI_COMBINER_HVECTOR:
    	    return YogiMPI_COMBINER_HVECTOR;
            break;
        case MPI_COMBINER_INDEXED:
            return YogiMPI_COMBINER_INDEXED;
            break;
        case MPI_COMBINER_HINDEXED_INTEGER:
            return YogiMPI_COMBINER_HINDEXED_INTEGER;
            break;
        case MPI_COMBINER_HINDEXED:
            return YogiMPI_COMBINER_HINDEXED;
            break;
        case MPI_COMBINER_INDEXED_BLOCK:
            return YogiMPI_COMBINER_INDEXED_BLOCK;
            break;
        case MPI_COMBINER_STRUCT_INTEGER:
            return YogiMPI_COMBINER_STRUCT_INTEGER;
            break;
        case MPI_COMBINER_STRUCT:
            return YogiMPI_COMBINER_STRUCT;
            break;
        case MPI_COMBINER_SUBARRAY:
            return YogiMPI_COMBINER_SUBARRAY;
            break;
        case MPI_COMBINER_DARRAY:
            return YogiMPI_COMBINER_DARRAY;
            break;
        case MPI_COMBINER_F90_REAL:
            return YogiMPI_COMBINER_F90_REAL;
            break;
        case MPI_COMBINER_F90_COMPLEX:
            return YogiMPI_COMBINER_F90_COMPLEX;
            break;
        case MPI_COMBINER_F90_INTEGER:
            return YogiMPI_COMBINER_F90_INTEGER;
            break;
        case MPI_COMBINER_RESIZED:
            return YogiMPI_COMBINER_RESIZED;
            break;
    }
} 

int YogiManager::errorToYogi(int mpiError) {
    std::map<int,int>::iterator it = yogiErrors.find(mpiError);
    if (it != yogiErrors.end()) return it->second;
    return YogiMPI_ERR_INTERN;
}

int YogiManager::errorToMPI(int yogiMPIError) {
    std::map<int,int>::iterator it = mpiErrors.find(yogiMPIError);
    if (it != mpiErrors.end()) return it->second;
    return MPI_ERR_INTERN;
}

int YogiManager::onesidedToMPI(int onesided) {
    switch(onesided) {
      case YogiMPI_MODE_NOCHECK:
          return MPI_MODE_NOCHECK;
          break;
      case YogiMPI_MODE_NOSTORE:
          return MPI_MODE_NOSTORE;
          break;
      case YogiMPI_MODE_NOPUT:
          return MPI_MODE_NOPUT;
          break;
      case YogiMPI_MODE_NOPRECEDE:
          return MPI_MODE_NOPRECEDE;
          break;
      case YogiMPI_MODE_NOSUCCEED:
          return MPI_MODE_NOSUCCEED;
          break;
      default:
          return onesided;
    }

}

int YogiManager::locktypeToMPI(int lock_type) {
    switch(lock_type) {
      case YogiMPI_LOCK_EXCLUSIVE:
          return MPI_LOCK_EXCLUSIVE;
          break;
      case YogiMPI_LOCK_SHARED:
          return MPI_LOCK_SHARED;
          break;
      default:
          return lock_type;
    }
}

int YogiManager::topoToYogi(int in_topo) {
    switch(in_topo) {
      case MPI_GRAPH:
          return YogiMPI_GRAPH;
          break;
      case MPI_CART:
          return YogiMPI_CART;
          break;
      case MPI_UNDEFINED:
          return YogiMPI_UNDEFINED;
          break;
      default:
          return in_topo;
    }
}

int YogiManager::comparisonToYogi(int mpiComp)
{
    std::map<int,int>::iterator it = yogiComps.find(mpiComp);
    if (it != yogiComps.end()) return it->second;
    return YogiMPI_UNEQUAL; 
}

int YogiManager::threadmodelToMPI(int threadmodel) {
    switch(threadmodel) {
      case YogiMPI_THREAD_SINGLE:
          return MPI_THREAD_SINGLE;
          break;
      case YogiMPI_THREAD_FUNNELED:
          return MPI_THREAD_FUNNELED;
          break;
      case YogiMPI_THREAD_SERIALIZED:
          return MPI_THREAD_SERIALIZED;
          break;
      case YogiMPI_THREAD_MULTIPLE:
          return MPI_THREAD_MULTIPLE;
          break;
      default:
          return threadmodel;
    }
}

int YogiManager::typeclassToMPI(int typeclass) {
    switch(typeclass) {
      case YogiMPI_TYPECLASS_REAL:
          return MPI_TYPECLASS_REAL; 
          break;
      case YogiMPI_TYPECLASS_INTEGER:
          return MPI_TYPECLASS_INTEGER;
          break;
      case YogiMPI_TYPECLASS_COMPLEX:
          return MPI_TYPECLASS_COMPLEX;
          break;
      default:
          return typeclass;
    }
}

int YogiManager::whenceToMPI(int whence) {
    switch(whence) {
      case YogiMPI_SEEK_SET:
          return MPI_SEEK_SET;
          break;
      case YogiMPI_SEEK_CUR:
          return MPI_SEEK_CUR;
          break;
      case YogiMPI_SEEK_END:
          return MPI_SEEK_END;
          break;
      default:
          return whence;
    }
}

int YogiManager::providedToYogi(int provided) {
    switch(provided) {
      case MPI_THREAD_SINGLE:
          return YogiMPI_THREAD_SINGLE;
          break;
      case MPI_THREAD_FUNNELED:
          return YogiMPI_THREAD_FUNNELED;
          break;
      case MPI_THREAD_SERIALIZED:
          return YogiMPI_THREAD_SERIALIZED;
          break;
      case MPI_THREAD_MULTIPLE:
          return YogiMPI_THREAD_MULTIPLE;
          break;
      default:
          return provided;
    }
}

int YogiManager::amodeToYogi(int amode) {
    switch(amode) {
      case MPI_MODE_RDONLY:
          return YogiMPI_MODE_RDONLY;
          break;
      case MPI_MODE_RDWR:
          return YogiMPI_MODE_RDWR;
          break;
      case MPI_MODE_WRONLY:
          return YogiMPI_MODE_WRONLY;
          break;
      case MPI_MODE_CREATE:
          return YogiMPI_MODE_CREATE;
          break;
      case MPI_MODE_EXCL:
          return YogiMPI_MODE_EXCL;
          break;
      case MPI_MODE_DELETE_ON_CLOSE:
          return YogiMPI_MODE_DELETE_ON_CLOSE;
          break;
      case MPI_MODE_UNIQUE_OPEN:
          return YogiMPI_MODE_UNIQUE_OPEN;
      case MPI_MODE_SEQUENTIAL:
          return YogiMPI_MODE_SEQUENTIAL;
          break;
      case MPI_MODE_APPEND:
          return YogiMPI_MODE_APPEND;
          break;
      default:
          return amode;
    }
}

int YogiManager::amodeToMPI(int amode) {
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
int YogiManager::rootToMPI(int root) {
    if (root == YogiMPI_ROOT) return MPI_ROOT;
    if (root == YogiMPI_PROC_NULL) return MPI_PROC_NULL;
    return root;
}

template <typename T>
int YogiManager::insertIntoPool(std::vector<T> &pool, T newItem, T marker,
                                int offset, int &counter) {

    /* First see if this already exists as a constant. If it does, just
       return the equivalent Yogi constant value. 
     */
    typename std::vector<T>::iterator it = pool.begin();
    it = std::find(pool.begin(), pool.begin() + offset, newItem);
    if (it != pool.begin() + offset) {
        return it - pool.begin(); 
    }
    int delta;

    /* Then see if the current counter exceeds the size of the vector.
       If it does, double it. */ 
    if (counter >= pool.capacity() - 1) {
#ifdef YOGI_DEBUG
        std::cout << "Counter at " << counter << ", doubling vector."
                  << std::endl;
#endif
        pool.resize(pool.capacity() * 2, marker);
    }
    // After the offset, find first instance of marker, replace with newItem.
    it = pool.begin();
    std::advance(it, offset);
    if (std::find(it, pool.end(), marker) != pool.end()) {
        delta = std::distance(pool.begin(), std::find(it, pool.end(), marker));
        pool.at(delta) = newItem;
    }
    else {
#ifdef YOGI_DEBUG
        std::cout << "Marker not found!" << std::endl;
#endif
        return -1;
    }
    // Bump up the counter and return the index.
    counter++; 
    return delta;
}

template <typename T>
void YogiManager::removeFromPool(std::vector<T> &pool, int index, T marker,
                                int offset, int &counter) {
    /* First see if the current index is at or above offset. If not, do not 
       make any modifications as these are considered read-only. */
    if (index < offset) return;
    // Replace the value at index with the marker value. 
    pool.at(index) = marker;
    // Decrement the counter. 
    counter--;

}

template <typename T>
T YogiManager::fetchFromPool(std::vector<T> &pool, int index) {
    return pool.at(index);  
}

MPI_Aint YogiManager::aintToMPI(YogiMPI_Aint in_aint) {
    return (MPI_Aint) in_aint;
}
   
MPI_Comm YogiManager::commToMPI(YogiMPI_Comm in_comm) {
    return fetchFromPool(commPool, in_comm);
}

MPI_Datatype YogiManager::datatypeToMPI(YogiMPI_Datatype in_data) {
    return fetchFromPool(datatypePool, in_data);
}

MPI_Errhandler YogiManager::errhandlerToMPI(YogiMPI_Errhandler in_errhandler) {
    return fetchFromPool(errPool, in_errhandler);
}

MPI_File YogiManager::fileToMPI(YogiMPI_File in_file) {
    return fetchFromPool(filePool, in_file);
}

MPI_Group YogiManager::groupToMPI(YogiMPI_Group in_group) {
    return fetchFromPool(groupPool, in_group);
}

MPI_Info YogiManager::infoToMPI(YogiMPI_Info in_info) {
    return fetchFromPool(infoPool, in_info);
}

MPI_Offset YogiManager::offsetToMPI(YogiMPI_Offset in_offset) {
    return (MPI_Offset) in_offset;
}

MPI_Op YogiManager::opToMPI(YogiMPI_Win in_op) {
    return fetchFromPool(opPool, in_op);
}

MPI_Request YogiManager::requestToMPI(YogiMPI_Request in_request) {
    return fetchFromPool(requestPool, in_request);
}

MPI_Status * YogiManager::statusToMPI(YogiMPI_Status * in_status) {
    /* This will grab the number of bytes needed.  We don't care about
     * structure padding since this area is never directly accessed by us.
     * It is ensured to be larger than we need.
    */
    return reinterpret_cast<MPI_Status *> (in_status->realStatus);
}

MPI_Win YogiManager::winToMPI(YogiMPI_Win in_win) {
    return fetchFromPool(winPool, in_win);
}

// Array conversions from Yogi handles to MPI handles

void YogiManager::requestToMPI(YogiMPI_Request * in_yogi,
                               MPI_Request * &out_mpi, int count) {
    int i;
    out_mpi = new MPI_Request[count];
    for (int i = 0; i < count; i++) {
        out_mpi[i] = requestToMPI(in_yogi[i]);
    }
}

void YogiManager::aintToMPI(YogiMPI_Aint * in_yogi, MPI_Aint * &out_mpi,
                            int count) {
    int i;
    out_mpi = new MPI_Aint[count];
    for (int i = 0; i < count; i++) {
        out_mpi[i] = aintToMPI(in_yogi[i]);
    }
}

void YogiManager::datatypeToMPI(YogiMPI_Datatype * in_yogi, 
                                MPI_Datatype * &out_mpi, int count) {
    int i;
    out_mpi = new MPI_Datatype[count];
    for (int i = 0; i < count; i++) {
        out_mpi[i] = datatypeToMPI(in_yogi[i]);
    }
}

/* Create MPI_Status arrays when required.  The MPI_Status objects remain
   uninitialized. */
void YogiManager::createStatus(MPI_Status * &out_mpi, int count) {
    out_mpi = new MPI_Status[count];
}

void YogiManager::createStatus(MPI_Status * &out_mpi, int *count) {
    out_mpi = new MPI_Status[*count];
}

// Conversions from MPI handles to Yogi handles

YogiMPI_Aint YogiManager::aintToYogi(MPI_Aint in_aint) {
    return (YogiMPI_Aint) in_aint;
}

YogiMPI_Comm YogiManager::commToYogi(MPI_Comm in_comm) {
    return insertIntoPool(commPool, in_comm, MPI_COMM_NULL, commOffset,
                          numComms);
}

YogiMPI_Datatype YogiManager::datatypeToYogi(MPI_Datatype in_data) {
    return insertIntoPool(datatypePool, in_data, MPI_DATATYPE_NULL,
                          datatypeOffset, numDatatypes);
}

YogiMPI_Errhandler YogiManager::errhandlerToYogi(MPI_Errhandler in_errhandler) {
    return insertIntoPool(errPool, in_errhandler, MPI_ERRHANDLER_NULL,
                          errOffset, numErrs);
}

YogiMPI_File YogiManager::fileToYogi(MPI_File in_file) {
    return insertIntoPool(filePool, in_file, MPI_FILE_NULL, fileOffset,
                          numFiles);

}

YogiMPI_Group YogiManager::groupToYogi(MPI_Group in_group) {
    return insertIntoPool(groupPool, in_group, MPI_GROUP_NULL, groupOffset,
                          numGroups);
}

YogiMPI_Info YogiManager::infoToYogi(MPI_Info in_info) {
    return insertIntoPool(infoPool, in_info, MPI_INFO_NULL, infoOffset,
                          numInfos);
}

YogiMPI_Offset YogiManager::offsetToYogi(MPI_Offset in_offset) {
    return (YogiMPI_Offset) in_offset;
}

YogiMPI_Op YogiManager::opToYogi(MPI_Op in_op) {
    return insertIntoPool(opPool, in_op, MPI_OP_NULL, opOffset, numOps);
}

YogiMPI_Request YogiManager::requestToYogi(MPI_Request in_request) {
    return insertIntoPool(requestPool, in_request, MPI_REQUEST_NULL,
                          requestOffset, numRequests);
}

YogiMPI_Status YogiManager::statusToYogi(MPI_Status &in_status) {
    YogiMPI_Status dest;
    dest.MPI_TAG = in_status.MPI_TAG;
    dest.MPI_SOURCE = in_status.MPI_SOURCE;
    dest.MPI_ERROR = in_status.MPI_ERROR;
    /* If this isn't the same address, force a memcpy */
    if ((void *)(dest.realStatus) != (void *)&in_status) {
        std::memcpy((void *)(dest.realStatus), (void *)&in_status,
                    sizeof(MPI_Status));
    }
    return dest;
}

YogiMPI_Win YogiManager::winToYogi(MPI_Win in_win) {
    return insertIntoPool(winPool, in_win, MPI_WIN_NULL, winOffset, numWins);
}

// Array conversion from MPI to Yogi

void YogiManager::requestToYogi(MPI_Request * &in_mpi, 
                                YogiMPI_Request * &out_yogi, int count,
                                bool free_mpi) {
    int i;
    for (int i = 0; i < count; i++) {
        out_yogi[i] = requestToYogi(in_mpi[i]);
    }
    if (free_mpi) freeRequest(in_mpi);
}

void YogiManager::aintToYogi(MPI_Aint * &in_mpi,
                             YogiMPI_Aint * &out_yogi, int count, 
                             bool free_mpi) {
    int i;
    for (int i = 0; i < count; i++) {
        out_yogi[i] = aintToYogi(in_mpi[i]);
    }
    if (free_mpi) freeAint(in_mpi);
}

void YogiManager::datatypeToYogi(MPI_Datatype * &in_mpi,
                                 YogiMPI_Datatype * &out_yogi, int count,
                                 bool free_mpi) {
    int i;
    for (int i = 0; i < count; i++) {
        out_yogi[i] = datatypeToYogi(in_mpi[i]);
    }
    if (free_mpi) freeDatatype(in_mpi);
}

void YogiManager::statusToYogi(MPI_Status * &in_mpi, YogiMPI_Status *& out_yogi,
                               int count, bool free_mpi) {
    int i;
    for (int i = 0; i < count; i++) {
        out_yogi[i] = statusToYogi(in_mpi[i]);
    }
    if (free_mpi) freeStatus(in_mpi);
}

// Removing Yogi handles

YogiMPI_Comm YogiManager::unmapComm(YogiMPI_Comm to_free) {
    removeFromPool(commPool, to_free, MPI_COMM_NULL, commOffset, numComms);
    return YogiMPI_COMM_NULL;
}

YogiMPI_Datatype YogiManager::unmapDatatype(YogiMPI_Datatype to_free) {
    removeFromPool(datatypePool, to_free, MPI_DATATYPE_NULL, datatypeOffset,
                   numDatatypes);
    return YogiMPI_DATATYPE_NULL;
}

YogiMPI_Errhandler YogiManager::unmapErrhandler(YogiMPI_Errhandler to_free)  {
    removeFromPool(errPool, to_free, MPI_ERRHANDLER_NULL, errOffset, numErrs);
    return YogiMPI_ERRHANDLER_NULL;
}

YogiMPI_File YogiManager::unmapFile(YogiMPI_File to_free) {
    removeFromPool(filePool, to_free, MPI_FILE_NULL, fileOffset, numFiles);
    return YogiMPI_FILE_NULL;
}

YogiMPI_Group YogiManager::unmapGroup(YogiMPI_Group to_free) {
    removeFromPool(groupPool, to_free, MPI_GROUP_NULL, groupOffset, numGroups);
    return YogiMPI_GROUP_NULL;
}

YogiMPI_Info YogiManager::unmapInfo(YogiMPI_Info to_free) {
    removeFromPool(infoPool, to_free, MPI_INFO_NULL, infoOffset, numInfos);
    return YogiMPI_INFO_NULL;
}

YogiMPI_Op YogiManager::unmapOp(YogiMPI_Op to_free) {
    removeFromPool(opPool, to_free, MPI_OP_NULL, opOffset, numOps);
    return YogiMPI_OP_NULL;
}

YogiMPI_Request YogiManager::unmapRequest(YogiMPI_Request to_free) {
    removeFromPool(requestPool, to_free, MPI_REQUEST_NULL, requestOffset, 
                   numRequests);
    return YogiMPI_REQUEST_NULL;
}

YogiMPI_Win YogiManager::unmapWin(YogiMPI_Win to_free) {
    removeFromPool(winPool, to_free, MPI_WIN_NULL, winOffset, numWins);
    return YogiMPI_WIN_NULL;
}

/* Used to deallocate temporary arrays.  Keeping these here to track memory 
   allocations (may implemented in future). */
void YogiManager::freeRequest(MPI_Request * &to_free) {
    delete[] to_free;
}

void YogiManager::freeAint(MPI_Aint * &to_free) {
    delete[] to_free;
}

void YogiManager::freeDatatype(MPI_Datatype * &to_free) {
    delete[] to_free;
}

void YogiManager::freeStatus(MPI_Status * &to_free) {
    delete[] to_free;
}