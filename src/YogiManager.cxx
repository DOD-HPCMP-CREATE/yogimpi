#include "YogiManager.h"
#include <algorithm>
#include <iterator>
#include <cstring>

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

int YogiManager::comparisonToYogi(int mpiComp)
{
    std::map<int,int>::iterator it = yogiComps.find(mpiComp);
    if (it != yogiComps.end()) return it->second;
    return YogiMPI_UNEQUAL; 
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
int YogiManager::insertIntoPool(std::vector<T> pool, T newItem, T marker,
                                int offset, int &counter) {
    int index;
    /* First see if the current counter exceeds the size of the vector.
       If it does, double it. */ 
    if (counter >= pool.size()) pool.resize(pool.size() * 2, marker);
    // After the offset, find first instance of marker, replace with newItem.
    typename std::vector<T>::iterator it = pool.begin();
    std::advance(it, offset);
    if (std::find(it, pool.end(), marker) != pool.end()) {
        *it = marker;
        index = std::distance(pool.begin(), it);
    }
    else {
        return -1;
    }
    // Bump up the counter and return the index.
    counter++; 
    return index;
}

template <typename T>
void YogiManager::removeFromPool(std::vector<T> pool, int index, T marker,
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
T YogiManager::fetchFromPool(std::vector<T> pool, int index) {
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

MPI_Status YogiManager::statusToMPI(YogiMPI_Status in_status) {
    /* This will grab the number of bytes needed.  We don't care about
     * structure padding since this area is never directly accessed by us.
     * It is ensured to be larger than we need.
    */
    MPI_Status convert;
    convert = (MPI_Status)in_status.realStatus;
    return convert; 
}

MPI_Win YogiManager::winToMPI(YogiMPI_Win in_win) {
    return fetchFromPool(winPool, in_win);
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

YogiMPI_Op YogiManager::opToYogi(MPI_Win in_op) {
    return insertIntoPool(opPool, in_op, MPI_OP_NULL, opOffset, numOps);
}

YogiMPI_Request YogiManager::requestToYogi(MPI_Request in_request) {
    return insertIntoPool(requestPool, in_request, MPI_REQUEST_NULL,
                          requestOffset, numRequests);
}

YogiMPI_Status YogiManager::statusToYogi(MPI_Status in_status) {
    YogiMPI_Status dest;
    dest.MPI_TAG = in_status.MPI_TAG;
    dest.MPI_SOURCE = in_status.MPI_SOURCE;
    dest.MPI_ERROR = in_status.MPI_ERROR;
    /* If this isn't the same address, force a memcpy */
    if ((void *)dest.realStatus != (void *)&in_status) {
        std::memcpy((void *)dest.realStatus, (void *)&in_status,
                    sizeof(MPI_Status));
    }
    return dest;
}

YogiMPI_Win YogiManager::winToYogi(MPI_Win in_win) {
    return insertIntoPool(winPool, in_win, MPI_WIN_NULL, winOffset, numWins);
}
