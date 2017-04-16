#ifndef _yogi_manager_included_
#define _yogi_manager_included_

#include "yogimpi.h"
#include "mpi.h"
#include <map>
#include <vector>

class YogiManager 
{
public: 
    static const int defaultPoolSize;

    static YogiManager* getInstance();
    int errorToYogi(int mpiError);
    int errorToMPI(int yogiMPIError);
    int comparisonToYogi(int mpiComp);
    int amodeToMPI(int amode);
    int rootToMPI(int root);

    MPI_Offset offsetToMPI(YogiMPI_Offset in_offset);
    MPI_Errhandler errhandlerToMPI(YogiMPI_Errhandler in_errhandler);
    MPI_Comm commToMPI(YogiMPI_Comm in_comm);
    MPI_Request requestToMPI(YogiMPI_Request in_request);
    MPI_Win winToMPI(YogiMPI_Win in_win);
    MPI_Op opToMPI(YogiMPI_Win in_op);
    MPI_Datatype datatypeToMPI(YogiMPI_Datatype in_data);
    MPI_Info infoToMPI(YogiMPI_Info in_info);
    MPI_Group groupToMPI(YogiMPI_Group in_group);
    MPI_File fileToMPI(YogiMPI_File in_file);
    MPI_Aint aintToMPI(YogiMPI_Aint in_aint);
    MPI_Status * statusToMPI(YogiMPI_Status in_status);

    YogiMPI_Offset offsetToYogi(MPI_Offset in_offset);
    YogiMPI_Errhandler errhandlerToYogi(MPI_Errhandler in_errhandler);
    YogiMPI_Comm commToYogi(MPI_Comm in_comm);
    YogiMPI_Request requestToYogi(MPI_Request in_request);
    YogiMPI_Win winToYogi(MPI_Win in_win);
    YogiMPI_Op opToYogi(MPI_Win in_op);
    YogiMPI_Datatype datatypeToYogi(MPI_Datatype in_data);
    YogiMPI_Info infoToYogi(MPI_Info in_info);
    YogiMPI_Group groupToYogi(MPI_Group in_group);
    YogiMPI_File fileToYogi(MPI_File in_file);
    YogiMPI_Aint aintToYogi(MPI_Aint in_aint);
    YogiMPI_Status statusToYogi(MPI_Status in_status);

protected: 
    YogiManager();
private:
    template <typename T> 
    int insertIntoPool(std::vector<T> pool, T newItem, T marker, int offset,
                       int &counter);
    template <typename T>
    void removeFromPool(std::vector<T> pool, int index, T marker, int offset,
                        int &counter);
    template <typename T>
    T fetchFromPool(std::vector<T> pool, int index);

    static YogiManager* _instance;
    std::map<int, int> yogiComps;
    std::map<int, int> mpiErrors;
    std::map<int, int> yogiErrors;

    std::vector<MPI_Errhandler> errPool;
    int numErrs;
    int errOffset;
    std::vector<MPI_Comm> commPool;
    int numComms;
    int commOffset;
    std::vector<MPI_Request> requestPool;
    int numRequests;
    int requestOffset;
    std::vector<MPI_Win> winPool;
    int numWins;
    int winOffset; 
    std::vector<MPI_Op> opPool;
    int numOps;
    int opOffset;
    std::vector<MPI_Datatype> datatypePool;
    int numDatatypes;
    int datatypeOffset;
    std::vector<MPI_Info> infoPool;
    int numInfos;
    int infoOffset;
    std::vector<MPI_Group> groupPool;
    int numGroups;
    int groupOffset;
    std::vector<MPI_File> filePool;
    int numFiles;
    int fileOffset;
};

#endif
