#include "yogimpi.h"
#include "YogiManager.h"
#include <map>

YogiManager* YogiManager::_instance = 0;

YogiManager* YogiManager::getInstance() {
    if (_instance == 0) {
        _instance = new YogiManager;
    }
    return _instance;
}

int YogiManager::initializeConstants() {
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

}

int YogiManager::errorToYogi(int mpiError) {
    std::map<int,int>::iterator it = yogiErrors.find(mpiError);
    if (it != yogiErrors.end()) return it->second;
    return YOGIMPI_ERR_INTERN;
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
