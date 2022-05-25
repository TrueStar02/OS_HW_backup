
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

    
    int usedMemory = 256 * 4096 + 0x200000;
    if(totalMemory < usedMemory) {
        myprintf("memory is too small, halt.\n");
        while(1)
            continue;
    }
    
    int freeMemory = totalMemory - usedMemory;

    int freePages = freeMemory / 4096;
    //int kernelPages = freePages / 2;
    //int userPages = freePages - kernelPages;
    int kernelPages = 256;
    int userPages = 256;

    int kernelPhysicalStartAddress = usedMemory;
    int userPhysicalStartAddress = usedMemory + kernelPages * 4096;

    int kernelPhysicalBitMapStart = 0x10000;
    int userPhysicalBitMapStart = kernelPhysicalBitMapStart + ceil(kernelPages / 8.0);
    int kernelVirtualBitMapStart = userPhysicalBitMapStart + ceil(userPages / 8.0);
    int swapBitMapStart = kernelVirtualBitMapStart + ceil(kernelPages / 8.0) + 32;

    kernelPhysical.initialize((char *)kernelPhysicalBitMapStart, kernelPages, kernelPhysicalStartAddress);
    userPhysical.initialize((char *)userPhysicalBitMapStart, userPages, userPhysicalStartAddress);
    kernelVirtual.initialize((char *)kernelVirtualBitMapStart,kernelPages + 256, 0xc0200000);
    swap.initialize((char *)swapBitMapStart,192);
    queue.initialize((uint32 *)0x100000);

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

    myprintf("kernel virtual pool\n"
           "    start address: 0x%x\n"
           "    total pages: %d  ( %d MB ) \n"
           "    bit map start address: 0x%x\n",
          0xc0200000,
           userPages, kernelPages * 4096 / 1024 / 1024,
           kernelVirtualBitMapStart);
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
    if(start == -1)
    {   
        int index = swap.allocate(1);
        
        
        if(index == -1)
            return 0;
        int vaddr = getOutPage();
        myprintf("vpage %p was swapped\n",vaddr);
        start = vaddr2paddr(vaddr);
        maindisk.write(512 + 8 * index,8,(void *)vaddr);
        memset((void *)vaddr,0,4096);
        uint32 * pte = (uint32 *)toPTE(vaddr);
        *pte = (index << 12)| 0x06;
        //myprintf("%p\n",*pte);

    }
    //myprintf("start:%p\n",start);
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
    //myprintf("%d pages released from physical pool,Addr:%p\n",count,paddr);
}

int MemoryManager::getTotalMemory()
{

    if(!totalMemory)
    {
        int memory = *((int *)0x7df0);
        
        int low = memory & 0xffff;
        
        int high = (memory >> 16) & 0xffff;

        totalMemory = low * 1024 + high * 64 * 1024;
        
    }

    return totalMemory;
}

void MemoryManager::openPageMechanism()
{
    int *directory = (int *)0x200000;
    int *page = (int *)(0x201000);


    memset(directory, 0, 0x1000);
    memset(page, 0, 0x1000);

    int address = 0;
    for (int i = 0; i < 512; ++i)
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

int MemoryManager::allocatePages(int type, const int count)
{
    int virtualAddress = allocateVirtualPages(type, count);
    
    if (!virtualAddress)
    {
        return 0;
    }
   
    bool flag;
    int physicalPageAddress;
    int vaddress = virtualAddress;

    for (int i = 0; i < count; ++i, vaddress += 4096)
    {
        flag = 0;
        physicalPageAddress = allocatePhysicalPages(type, 1);
        if (physicalPageAddress)
        {
            flag = connectPhysicalVirtualPage(vaddress, physicalPageAddress);
            queue.push(vaddress);
        }
        else 
        {
            flag = false;
        }

        if (!flag)
        {
            releasePages(type, virtualAddress, i);
            releaseVirtualPages(type, virtualAddress + i * 4096, count - i);
            myprintf("Failed\n");
            return 0;
        }
        
    }
    myprintf("%d pages allocated from virtual pool,Addr:%p\n",count,virtualAddress);
    return virtualAddress;
}



int MemoryManager::allocateVirtualPages(int type, const int count)
{
    int start = -1;

    if (type == 0)
    {
        start = kernelVirtual.allocate(count);
    }

    return (start == -1) ? 0 : start;
}

bool MemoryManager::connectPhysicalVirtualPage(const int virtualAddress, const int physicalPageAddress)
{
    int *pde = (int *)toPDE(virtualAddress);
    int *pte = (int *)toPTE(virtualAddress);
    //myprintf("%p\n",*pde);
    if(!(*pde & 0x00000001)) 
    {
        int page = allocatePhysicalPages(0, 1);
        myprintf("Allocate pagetable\n");
        if (!page)
            return false;

        *pde = page | 0x7;
        char *pagePtr = (char *)(((int)pte) & 0xfffff000);
        memset(pagePtr, 0, 4096);
    }

    //myprintf("vpage %p was connected to page %p\n",virtualAddress,physicalPageAddress);
    *pte = physicalPageAddress | 0x7;

    return true;
}

int MemoryManager::toPDE(const int virtualAddress)
{
    return (0xfffff000 + (((virtualAddress & 0xffc00000) >> 22) * 4));
}

int MemoryManager::toPTE(const int virtualAddress)
{
    return (0xffc00000 + ((virtualAddress & 0xffc00000) >> 10) + (((virtualAddress & 0x003ff000) >> 12) * 4));
}

void MemoryManager::releasePages(int type, const int virtualAddress, const int count)
{
    int vaddr = virtualAddress;
    int *pte;
    for (int i = 0; i < count; ++i, vaddr += 4096)
    {
        pte = (int *)toPTE(vaddr);
        if((*pte) & 1)
            releasePhysicalPages(type, vaddr2paddr(vaddr), 1);
        else
            swap.release((*pte)>>12,1);

        *pte = 0;
    }

    releaseVirtualPages(type, virtualAddress, count);
}

int MemoryManager::vaddr2paddr(int vaddr)
{
    int *pte = (int *)toPTE(vaddr);
    int page = (*pte) & 0xfffff000;
    int offset = vaddr & 0xfff;
    return (page + offset);
}

void MemoryManager::releaseVirtualPages(int type, const int vaddr, const int count)
{
    if (type == 0)
    {
        kernelVirtual.release(vaddr, count);
        myprintf("%d pages released from virtual pool,Addr:%p\n",count,vaddr);
    }
}

uint32 MemoryManager::getOutPage()
{
    int * pte;
    uint32 vaddr;

    do
    {
        vaddr=queue.pop();
        pte = (int *)toPTE(vaddr);

    } while (((*pte) & 1) == 0);

    return vaddr;
}

void * BUFFER_POS = (void *)0xe000;
extern "C" void c_pagefault_handler(uint32 vpage,uint32 errcode)
{   
    if(errcode & 1)
    {
        myprintf("Permission Error!\n");
        while(1)
            continue;
    }
    else
    {   myprintf("Page Fault:%p\n",vpage);
        vpage = vpage & 0xfffff000;
        uint32 * vpte =(uint32 *)memoryManager.toPTE(vpage);
        uint32 index = (*vpte) >> 12;
        maindisk.read((index << 3) + 512,8,BUFFER_POS);
        int outpage = memoryManager.getOutPage();
        myprintf("vpage %p was swapped\n",outpage);
        uint32 *  outpte = (uint32 *)memoryManager.toPTE(outpage);
        maindisk.write((index << 3) + 512,8,(void *)outpage);
        memcpy((void *)outpage,BUFFER_POS,4096);
        uint32 temp = *vpte;
        *vpte = *outpte;
        *outpte = temp;

    }
}