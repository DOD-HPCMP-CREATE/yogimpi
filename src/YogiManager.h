#ifndef _yogi_manager_included_
#define _yogi_manager_included_

#include "yogimpi.h"
#include <map>

class YogiManager 
{
public: 
    static YogiManager* getInstance();
    int errorToYogi(int mpiError);
    int errorToMPI(int yogiMPIError);
    int comparisonToYogi(int mpiComp);
    int amodeToMPI(int amode);
    int rootToMPI(int root);

    MPI_Offset toMPI(YogiMPI_Offset in_offset);
    MPI_Errhandler toMPI(YogiMPI_Errhandler in_errhandler);
    MPI_Comm toMPI(YogiMPI_Comm in_comm);
    MPI_Request toMPI(YogiMPI_Request in_request);
    MPI_Win toMPI(YogiMPI_Win in_win);
    MPI_Op toMPI(YogiMPI_Win in_op);
    MPI_Datatype toMPI(YogiMPI_Datatype in_data);
    MPI_Info toMPI(YogiMPI_Info in_info);
    MPI_Group toMPI(YogiMPI_Group in_group);
    MPI_File toMPI(YogiMPI_File in_file);
    MPI_Aint toMPI(YogiMPI_Aint in_aint);
    MPI_Status toMPI(YogiMPI_Status in_status);

    YogiMPI_Offset toYogi(MPI_Offset in_offset);
    YogiMPI_Errhandler toYogi(MPI_Errhandler in_errhandler);
    YogiMPI_Comm toYogi(MPI_Comm in_comm);
    YogiMPI_Request toYogi(MPI_Request in_request);
    YogiMPI_Win toYogi(MPI_Win in_win);
    YogiMPI_Op toYogi(MPI_Win in_op);
    YogiMPI_Datatype toYogi(MPI_Datatype in_data);
    YogiMPI_Info toYogi(MPI_Info in_info);
    YogiMPI_Group toYogi(MPI_Group in_group);
    YogiMPI_File toYogi(MPI_File in_file);
    YogiMPI_Aint toYogi(MPI_Aint in_aint);
    YogiMPI_Status toYogi(MPI_Status in_status);

protected: 
    YogiManager();
private:
    static YogiManager* _instance;
    std::map<int, int> yogiComps;
    std::map<int, int> mpiErrors;
    std::map<int, int> yogiErrors;
};

#endif
