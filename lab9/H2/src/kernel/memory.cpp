
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
        myprintk("memory is too small, halt.\n");
        while(1)
            continue;
    }
    
    int freeMemory = totalMemory - usedMemory;

    int freePages = freeMemory / 4096;
    int kernelPages = freePages / 2;
    int userPages = freePages - kernelPages;
    //int kernelPages = 256;
    //int userPages = 256;

    int kernelPhysicalStartAddress = usedMemory;
    int userPhysicalStartAddress = usedMemory + kernelPages * 4096;

    int kernelPhysicalBitMapStart = 0xc0010000;
    int userPhysicalBitMapStart = kernelPhysicalBitMapStart + ceil(kernelPages / 8.0);
    int kernelVirtualBitMapStart = userPhysicalBitMapStart + ceil(userPages / 8.0);
    int swapBitMapStart = kernelVirtualBitMapStart + ceil(kernelPages / 8.0) + 32;

    kernelPhysical.initialize((char *)kernelPhysicalBitMapStart, kernelPages, kernelPhysicalStartAddress);
    userPhysical.initialize((char *)userPhysicalBitMapStart, userPages, userPhysicalStartAddress);
    kernelVirtual.initialize((char *)kernelVirtualBitMapStart,kernelPages + 256, 0xc0200000);
    swap.initialize((char *)swapBitMapStart,192);
    kernelQueue.initialize((uint32 *)0xc0100000,32768);
    userQueue.initialize((uint32 *)0xc0180000,32768);

    myprintk("total memory: %d bytes ( %d MB )\n", 
            totalMemory, 
            totalMemory / 1024 / 1024);

    myprintk("kernel pool\n"
           "    start address: 0x%x\n"
           "    total pages: %d ( %d MB )\n"
           "    bitmap start address: 0x%x\n",
           kernelPhysicalStartAddress, 
           kernelPages, kernelPages * 4096 / 1024 / 1024,
           kernelPhysicalBitMapStart);

    myprintk("user pool\n"
           "    start address: 0x%x\n"
           "    total pages: %d ( %d MB )\n"
           "    bit map start address: 0x%x\n",
           userPhysicalStartAddress, 
           userPages, userPages * 4096 / 1024 / 1024,
           userPhysicalBitMapStart);

    myprintk("kernel virtual pool\n"
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
        int vaddr = getOutPage(type);
        myprintf("vpage %p was swapped\n",vaddr);
        if(type)
        {
            int pcount = vaddr & 0xfff;
            vaddr &= 0xfffff000;
            int pid = programManager.queryByCount(pcount);
            uint32 pt_paddr = ((uint32 *)(programManager.findbyPID(pid)->pageDirectoryAddress))[vaddr >> 22];
            uint32 pt_temp_vaddr = allocateVirtualPages(1,1);
            connectPhysicalVirtualPage(pt_temp_vaddr,pt_paddr);
            uint32 page_paddr = ((uint32 *)pt_temp_vaddr)[(vaddr >> 12) & 0x3ff] & 0xfffff000;
            uint32 page_temp_vaddr = allocateVirtualPages(1,1);
            connectPhysicalVirtualPage(page_temp_vaddr,page_paddr);
            asm_system_call(3,(index << 3) + 512,8,page_temp_vaddr);
            memset((void *)page_temp_vaddr,0,4096);
            releaseVirtualPages(1,page_temp_vaddr,1);
            ((uint32 *)pt_temp_vaddr)[(vaddr >> 12) & 0x3ff] = (index << 12) | 0x06;
            releaseVirtualPages(1,pt_temp_vaddr,1);
            start = page_paddr;
        }
        else
        {   
            start = vaddr2paddr(vaddr);
            asm_system_call(3,512 + 8 * index,8,vaddr);
            memset((void *)vaddr,0,4096);
            uint32 * pte = (uint32 *)toPTE(vaddr);
            *pte = (index << 12) | 0x06;
            //myprintf("%p\n",*pte);
        }

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

int MemoryManager::allocatePages(int type, const int count,bool forbidSwap)
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
            if(!forbidSwap)
            {   if(type)
                    userQueue.push(vaddress | programManager.running->count);
                else
                    kernelQueue.push(vaddress);
            }
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
    else if (type == 1)
    {
        start = programManager.running->userVirtual.allocate(count);
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
    else if (type == 1)
    {
        programManager.running->userVirtual.release(vaddr, count);
    }
}

uint32 MemoryManager::getOutPage(int type)
{
    int * pte;
    uint32 vaddr;
    if(type)
    {   bool flag = 0;
        uint32 temp_vaddr = allocateVirtualPages(1,1);
        do
        {   vaddr = userQueue.pop();
            int pcount = vaddr & 0xfff;
            int pid = programManager.queryByCount(pcount);
            if(pcount == -1)
                continue;
            uint32 pt_paddr = ((uint32 *)(programManager.findbyPID(pid)->pageDirectoryAddress))[vaddr >> 22];
            
            connectPhysicalVirtualPage(temp_vaddr,pt_paddr);
            flag = ((uint32 *)temp_vaddr)[(vaddr >> 12) & 0x3ff] & 1;
            *(uint32 *)toPTE(temp_vaddr) = 0;
            
        } while (!flag);
        releaseVirtualPages(1,temp_vaddr,1);
    }
    else
    {
        do
        {
            vaddr = kernelQueue.pop();
            pte = (int *)toPTE(vaddr);

        } while (((*pte) & 1) == 0);
    }
    return vaddr;
}

void * BUFFER_POS = (void *)0xc000e000;
extern "C" void c_pagefault_handler(uint32 vpage,uint32 errcode)
{   
    if(errcode & 1)
    {
        myprintf("Permission Error!\n");
        while(1)
            continue;
    }
    else
    {   myprintf("Page Fault:%p,%p\n",vpage,errcode);
        vpage = vpage & 0xfffff000;
        uint32 * vpte =(uint32 *)memoryManager.toPTE(vpage);
        uint32 index = (*vpte) >> 12;
        maindisk.read((index << 3) + 512,8,BUFFER_POS);
        int type = ((errcode >> 2) & 1);
        int outpage = memoryManager.getOutPage(type);
        if(type)
        {   
            int pcount = outpage & 0xfff;
            outpage &= 0xfffff000;
            int pid = programManager.queryByCount(pcount);
            uint32 pt_paddr = ((uint32 *)(programManager.findbyPID(pid)->pageDirectoryAddress))[outpage >> 22];
            uint32 pt_temp_vaddr = memoryManager.allocateVirtualPages(1,1);
            memoryManager.connectPhysicalVirtualPage(pt_temp_vaddr,pt_paddr);
            uint32 page_paddr = ((uint32 *)pt_temp_vaddr)[(outpage >> 12) & 0x3ff] & 0xfffff000;
            uint32 page_temp_vaddr = memoryManager.allocateVirtualPages(1,1);
            memoryManager.connectPhysicalVirtualPage(page_temp_vaddr,page_paddr);
            maindisk.write((index << 3) + 512,8,(void *) page_temp_vaddr);
            memcpy((void *)page_temp_vaddr,BUFFER_POS,4096);
            memoryManager.releaseVirtualPages(1,page_temp_vaddr,1);
            uint32 temp = *vpte;
            *vpte = ((uint32 *)pt_temp_vaddr)[(outpage >> 12) & 0x3ff];
            ((uint32 *)pt_temp_vaddr)[(outpage >> 12) & 0x3ff] = temp;
            memoryManager.releaseVirtualPages(1,pt_temp_vaddr,1);

        }
        else
        {
            myprintf("vpage %p was swapped\n",outpage);
            uint32 *  outpte = (uint32 *)memoryManager.toPTE(outpage);
            maindisk.write((index << 3) + 512,8,(void *)outpage);
            memcpy((void *)outpage,BUFFER_POS,4096);
            uint32 temp = *vpte;
            *vpte = *outpte;
            *outpte = temp;
        }
        

    }
}