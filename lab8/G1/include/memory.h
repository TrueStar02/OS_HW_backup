#ifndef MEMORY_H_
#define MEMORY_H_

#include "addrpool.h"

class MemoryManager
{  //0 pk 1 pu 2 vk 
   private:
        int totalMemory;
        AddressPool kernelPhysical;
        AddressPool userPhysical;

    public:
        MemoryManager();
        void initialize();
        int allocatePhysicalPages(int type, const int count);
        void releasePhysicalPages(int type, const int startAddress, const int count);
        int getTotalMemory();
        void openPageMechanism();
};

#endif