#ifndef _yogi_manager_included_
#define _yogi_manager_included_

#include "yogimpi.h"
#include "mpi.h"
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

class YogiManager 
{
public: 
    static const int defaultPoolSize;

    static YogiManager* getInstance();

    void setGlobalRank(int rank);
    void openDebugLog();
    void closeDebugLog();
    void writeToDebugLog(const char * toWrite);
    void loadMPILibrary();
    int errorToMPI(int yogiMPIError);
    int amodeToMPI(int amode);
    int rootToMPI(int root);
    int onesidedToMPI(int onesided);
    int locktypeToMPI(int lock_type);
    int winattrToMPI(int win_attr);
    int threadmodelToMPI(int threadmodel);
    int typeclassToMPI(int typeclass);
    int whenceToMPI(int whence);
    int commattrToMPI(int comm_attr);
 
    int errorToYogi(int mpiError);
    int comparisonToYogi(int mpiComp);
    int providedToYogi(int provided);
    int amodeToYogi(int amode);
    int topoToYogi(int in_topo);
    int combinerToYogi(int in_combiner);

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
    MPI_Count countToMPI(YogiMPI_Count in_count);
    MPI_Status * statusToMPI(YogiMPI_Status * in_status);

    // Create MPI_Status arrays when required
    void createStatus(MPI_Status *&, int);
    void createStatus(MPI_Status *&, int *);
  
    // Array-conversion to MPI
    void requestToMPI(YogiMPI_Request *, MPI_Request *&, int);
    void aintToMPI(YogiMPI_Aint *, MPI_Aint *&, int);
    void datatypeToMPI(YogiMPI_Datatype *, MPI_Datatype *&, int);

    YogiMPI_Offset offsetToYogi(MPI_Offset in_offset);
    YogiMPI_Errhandler errhandlerToYogi(MPI_Errhandler in_errhandler);
    YogiMPI_Comm commToYogi(MPI_Comm in_comm);
    YogiMPI_Request requestToYogi(MPI_Request in_request);
    YogiMPI_Win winToYogi(MPI_Win in_win);
    YogiMPI_Op opToYogi(MPI_Op in_op);
    YogiMPI_Datatype datatypeToYogi(MPI_Datatype in_data);
    YogiMPI_Info infoToYogi(MPI_Info in_info);
    YogiMPI_Group groupToYogi(MPI_Group in_group);
    YogiMPI_File fileToYogi(MPI_File in_file);
    YogiMPI_Aint aintToYogi(MPI_Aint in_aint);
    YogiMPI_Count countToYogi(MPI_Count in_count);
    YogiMPI_Status statusToYogi(MPI_Status &in_status, bool set_error = true);

    // Array-conversion to Yogi
    void requestToYogi(MPI_Request * &in_mpi, YogiMPI_Request *& out_yogi,
                       int count, bool free_mpi = false);
    void aintToYogi(MPI_Aint * &in_mpi, YogiMPI_Aint *& out_yogi, 
                    int count, bool free_mpi = false);
    void datatypeToYogi(MPI_Datatype * &in_mpi, YogiMPI_Datatype *& out_yogi,
                        int count, bool free_mpi = false);
    void statusToYogi(MPI_Status * &in_mpi, YogiMPI_Status *& out_yogi,
                      int count, bool free_mpi = false);

    void freeRequest(MPI_Request * &to_free);
    void freeAint(MPI_Aint * &to_free);
    void freeDatatype(MPI_Datatype * &to_free);
    void freeStatus(MPI_Status * &to_free); 

    YogiMPI_Comm unmapComm(YogiMPI_Comm to_free); 
    YogiMPI_Datatype unmapDatatype(YogiMPI_Datatype to_free);
    YogiMPI_Errhandler unmapErrhandler(YogiMPI_Errhandler to_free);
    YogiMPI_File unmapFile(YogiMPI_File to_free);
    YogiMPI_Group unmapGroup(YogiMPI_Group to_free);
    YogiMPI_Info unmapInfo(YogiMPI_Info to_free);
    YogiMPI_Op unmapOp(YogiMPI_Op to_free);
    YogiMPI_Request unmapRequest(YogiMPI_Request to_free);
    YogiMPI_Win unmapWin(YogiMPI_Win to_free);

protected: 
    YogiManager();
private:
    template <typename T, typename V>
    int insertIntoPool(std::vector<T> &pool, T newItem, V marker_in, int offset,
                       int &counter);

    template <typename T, typename V>
    void removeFromPool(std::vector<T> &pool, int index, V marker_in,
                        int offset, int &counter);

    template <typename T>
    T fetchFromPool(std::vector<T> &pool, int index);

    static YogiManager* _instance;

    int globalRank;
    std::ofstream debugLogFile;

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
    void *libraryHandle;
};

#endif
