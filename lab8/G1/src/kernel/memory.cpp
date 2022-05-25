
#include "tools.h"
#include "asm_utils.h"
#include "screen.h"
#include "program.h"
#include "os_extern.h"
#include "memory.h"

MemoryManager::MemoryManager() {
    initialize();
}

void MemoryManager::initialize()
{
    totalMemory = 0;
    totalMemory = getTotalMemory();

    
    int usedMemory = 256 * 4096 + 0x100000;
    if(this->totalMemory < usedMemory) {
        myprintf("memory is too small, halt.\n");
        while(1)
            continue;
    }
    
    int freeMemory = this->totalMemory - usedMemory;

    int freePages = freeMemory / 4096;
    int kernelPages = freePages / 2;
    int userPages = freePages - kernelPages;

    int kernelPhysicalStartAddress = usedMemory;
    int userPhysicalStartAddress = usedMemory + kernelPages * 4096;

    int kernelPhysicalBitMapStart = 0x10000;
    int userPhysicalBitMapStart = kernelPhysicalBitMapStart + ceil(kernelPages / 8.0);

    kernelPhysical.initialize((char *)kernelPhysicalBitMapStart, kernelPages, kernelPhysicalStartAddress);
    userPhysical.initialize((char *)userPhysicalBitMapStart, userPages, userPhysicalStartAddress);

    myprintf("total memory: %d bytes ( %d MB )\n", 
            totalMemory, 
            totalMemory / 1024 / 1024);

    myprintf("kernel pool\n"
           "    start address: 0x%x\n"
           "    total pages: %d ( %d MB )\n"
           "    bitmap start address: 0x%x\n",
           kernelPhysicalStartAddress, 
           kernelPages, kernelPages * 4096 / 1024 / 1024,
           kernelPhysicalBitMapStart);

    myprintf("user pool\n"
           "    start address: 0x%x\n"
           "    total pages: %d ( %d MB )\n"
           "    bit map start address: 0x%x\n",
           userPhysicalStartAddress, 
           userPages, userPages * 4096 / 1024 / 1024,
           userPhysicalBitMapStart);
}

int MemoryManager::allocatePhysicalPages(int type, const int count)
{
    int start = -1;

    if (type == 0)
    {
        start = kernelPhysical.allocate(count);
    }
    else if (type == 1)
    {
        start = userPhysical.allocate(count);
    }

    return (start == -1) ? 0 : start;
}

void MemoryManager::releasePhysicalPages(int type, const int paddr, const int count)
{
    if (type == 0)
    {
        kernelPhysical.release(paddr, count);
    }
    else if (type == 1)
    {

        userPhysical.release(paddr, count);
    }
}

int MemoryManager::getTotalMemory()
{

    if(!this->totalMemory)
    {
        int memory = *((int *)0x7df0);
        
        int low = memory & 0xffff;
        
        int high = (memory >> 16) & 0xffff;

        this->totalMemory = low * 1024 + high * 64 * 1024;
        
    }

    return this->totalMemory;
}

void MemoryManager::openPageMechanism()
{
    int *directory = (int *)0x100000;
    int *page = (int *)(0x101000);


    memset(directory, 0, 0x1000);
    memset(page, 0, 0x1000);

    int address = 0;
    for (int i = 0; i < 256; ++i)
    {
        page[i] = address | 0x7;
        address += 0x1000;
    }


    directory[0] = ((int)page) | 0x07;
    directory[768] = directory[0];
    directory[1023] = ((int)directory) | 0x7;

    asm_init_page_reg(directory);

    myprintf("open page mechanism\n");
    
}