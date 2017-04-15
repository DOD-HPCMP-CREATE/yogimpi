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

MPI_Offset YogiManager::toMPI(YogiMPI_Offset in_offset) {

}

MPI_Errhandler YogiManager::toMPI(YogiMPI_Errhandler in_errhandler) {

}

MPI_Comm YogiManager::toMPI(YogiMPI_Comm in_comm) {

}

MPI_Request YogiManager::toMPI(YogiMPI_Request in_request) {

}

MPI_Win YogiManager::toMPI(YogiMPI_Win in_win) {

}

MPI_Op YogiManager::toMPI(YogiMPI_Win in_op) {

}

MPI_Datatype YogiManager::toMPI(YogiMPI_Datatype in_data) {

}

MPI_Info YogiManager::toMPI(YogiMPI_Info in_info) {

}

MPI_Group YogiManager::toMPI(YogiMPI_Group in_group) {

}

MPI_File YogiManager::toMPI(YogiMPI_File in_file) {

}

MPI_Aint YogiManager::toMPI(YogiMPI_Aint in_aint) {

}

MPI_Status YogiManager::toMPI(YogiMPI_Status in_status) {
    /* This will grab the number of bytes needed.  We don't care about
     * structure padding since this area is never directly accessed by us.
     * It is ensured to be larger than we need.
    */
    return (MPI_Status)&in_status->realStatus[0];
}

YogiMPI_Offset YogiManager::toYogi(MPI_Offset in_offset) {

}

YogiMPI_Errhandler YogiManager::toYogi(MPI_Errhandler in_errhandler) {

}

YogiMPI_Comm YogiManager::toYogi(MPI_Comm in_comm) {

}

YogiMPI_Request YogiManager::toYogi(MPI_Request in_request) {

}

YogiMPI_Win YogiManager::toYogi(MPI_Win in_win) {

}

YogiMPI_Op YogiManager::toYogi(MPI_Win in_op) {

}

YogiMPI_Datatype YogiManager::toYogi(MPI_Datatype in_data) {

}

YogiMPI_Info YogiManager::toYogi(MPI_Info in_info) {

}

YogiMPI_Group YogiManager::toYogi(MPI_Group in_group) {

}

YogiMPI_File YogiManager::toYogi(MPI_File in_file) {

}

YogiMPI_Aint YogiManager::toYogi(MPI_Aint in_aint) {

}

YogiMPI_Status YogiManager::toYogi(MPI_Status in_status) {
    dest->MPI_TAG = source->MPI_TAG;
    dest->MPI_SOURCE = source->MPI_SOURCE;
    dest->MPI_ERROR = source->MPI_ERROR;
    /* If this isn't the same address, force a memcpy */
    if ((void *)dest->realStatus != (void *)source) {
        memcpy((void *)dest->realStatus, (void *)source, sizeof(MPI_Status));
    }
}


