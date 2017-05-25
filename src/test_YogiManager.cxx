#include "YogiManager.h"
#include <iostream>
#include <algorithm>

int main() {
    YogiManager *manager = YogiManager::getInstance();
    MPI_Status blah;
    blah.MPI_SOURCE = 1;
    blah.MPI_TAG = 1001;
    blah.MPI_ERROR = 0;
    YogiMPI_Status yogi = manager->statusToYogi(blah);
    std::cout << "Source: " << yogi.MPI_SOURCE <<std::endl;    
    std::cout << "Tag: " << yogi.MPI_TAG <<std::endl;
    std::cout << "Error: " << yogi.MPI_ERROR <<std::endl;
    MPI_Status *another = manager->statusToMPI(&yogi);
    std::cout << "Source: " << another->MPI_SOURCE << std::endl;
    std::cout << "Tag: " << another->MPI_TAG << std::endl;
    std::cout << "Error: " << another->MPI_ERROR << std::endl;
}
