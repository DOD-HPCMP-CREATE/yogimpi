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
protected: 
    YogiManager();
private:
    static YogiManager* _instance;
    std::map<int, int> yogiComps;
    std::map<int, int> mpiErrors;
    std::map<int, int> yogiErrors;
};

#endif
