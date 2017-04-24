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
    MPI_Datatype blahs[200];
    YogiMPI_Datatype yogi_blahs[200];
    int i;
    int j = 1000;
    for (i = 0; i < 200; i++) {
        blahs[i] = j;
        j++;
    } 
    for (i = 0; i < 200; i++) {
        yogi_blahs[i] = manager->datatypeToYogi(blahs[i]);
    }
    for (i = 0; i < 200; i++) {
        MPI_Datatype a_data = manager->datatypeToMPI(yogi_blahs[i]);
        if (a_data != blahs[i]) {
            std::cout << a_data << " doesn't match " << blahs[i] << std::endl;
        }
    } 
    std::cout << "Removing datatype at index " << yogi_blahs[20] << std::endl;
    manager->unmapDatatype(yogi_blahs[20]);
    std::cout << "Received new insertion index " << manager->datatypeToYogi(3000) << std::endl;
    std::cout << "Value at location " << yogi_blahs[20] << ": " << 
              manager->datatypeToMPI(yogi_blahs[20]) << std::endl;
}
