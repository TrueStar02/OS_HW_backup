#ifndef MEMORY_H_
#define MEMORY_H_

#include "addrpool.h"
#include "queue.h"

class MemoryManager
{  //0 pk 1 pu 2 vk 
   private:
        int totalMemory;
        AddressPool kernelPhysical;
        AddressPool userPhysical;
        AddressPool kernelVirtual;
        BitMap swap;
        Queue userQueue;
        Queue kernelQueue;

    public:
        MemoryManager();
        void initialize();
        int allocatePhysicalPages(int type, const int count);
        void releasePhysicalPages(int type, const int startAddress, const int count);
        int getTotalMemory();
        int allocatePages(int type, const int count,bool forbidSwap = 0);
        int allocateVirtualPages(int type, const int count);
        bool connectPhysicalVirtualPage(const int virtualAddress, const int physicalPageAddress);
        uint32 getOutPage(int type);
        int toPDE(const int virtualAddress);
        int toPTE(const int virtualAddress);
        void releasePages(int type, const int virtualAddress, const int count); 
        int vaddr2paddr(int vaddr);
        void releaseVirtualPages(int type, const int vaddr, const int count);
};

#endif