#include "program.h"
#include "tools.h"
#include "interrupt.h"
#include "asm_utils.h"
#include "screen.h"
#include "os_extern.h"
               
char PCB_SET[65536]; 
bool PCB_SET_STATUS[16];     
int processCount = 0;
int table[4096];

ProgramManager::ProgramManager()
{
    initialize();
}
void ProgramManager::initialize()
{
    allPrograms.initialize();
    readyPrograms.initialize();
    running = nullptr;

    for (int i = 0; i < 16; ++i)
    {
        PCB_SET_STATUS[i] = 0;
    }

    for(int i = 0;i < 4096;i++)
        table[i] = -1;

    int selector;

    selector = asm_add_global_descriptor(0x0000ffff, 0x00cff800);
    USER_CODE_SELECTOR = (selector << 3) | 0x3;

    selector = asm_add_global_descriptor(0x0000ffff, 0x00cff200);
    USER_DATA_SELECTOR = (selector << 3) | 0x3;

    selector = asm_add_global_descriptor(0x00000000, 0x0040f600);
    USER_STACK_SELECTOR = (selector << 3) | 0x3;

    initializeTSS();
}
PCB * ProgramManager::findbyPID(int pid)
{   if(PCB_SET_STATUS[pid])
        return (PCB *)((int)PCB_SET + (pid << 12));
    else
        return nullptr;
}
int ProgramManager::queryByCount(int c)
{
    return table[c];
}
int ProgramManager::executeThread(ThreadFunction function, void *parameter, const char *name, int priority)
{
    bool status = irm.getInterruptStatus();
    irm.disableInterrupt();

    
    PCB *thread = allocatePCB();

    if (!thread)
        return -1;


    
    memset(thread,0,4096);

    for (int i = 0; i < 20 && name[i]; ++i)
        thread->name[i] = name[i];

    thread->status = 1;
    thread->priority = priority;
    thread->ticks =  10;
    thread->ticksPassedBy = 0;
    thread->pid = ((int)thread - (int)PCB_SET) / 4096;

    thread->stack = (int *)((int)thread + 4096 - sizeof(ProcessStartStack));
    thread->stack -= 8;
    thread->stack[0] = 0;
    thread->stack[1] = 0;
    thread->stack[2] = 0;
    thread->stack[3] = 0;
    thread->stack[4] = 0;
    thread->stack[5] = (int)function;
    thread->stack[6] = (int)program_exit;
    thread->stack[7] = (int)parameter;
    allPrograms.push_back(&(thread->tagInAllList));
    readyPrograms.push_back(&(thread->tagInGeneralList));
    
    if(status)
        irm.enableInterrupt();
    return thread->pid;
}
void ProgramManager::schedule()
{
    bool status = irm.getInterruptStatus();
    irm.disableInterrupt();

    if (readyPrograms.size() == 0)
    {
        if(status)
            irm.enableInterrupt();
        return;
    }

    if (running->status == 2)
    {   
        running->status = 1;
        running->ticks = 10;
        readyPrograms.push_back(&(running->tagInGeneralList));
    }
    else if (running->status == 4)
        releasePCB(running);

    ListItem *item = readyPrograms.front();
    PCB *next = (PCB *)((int)item-(int)&(((PCB *)0)->tagInGeneralList));
    PCB *cur = running;
    next->status = 2;
    running = next;
    readyPrograms.pop_front();

    activateProgramPage(next);
    asm_switch_thread(cur, next);

    if(status)
        irm.enableInterrupt();
}
void ProgramManager::WakeUp(PCB * program)
{   program->status = 1;
    readyPrograms.push_front(&(program->tagInGeneralList));
}
PCB *ProgramManager::allocatePCB()
{
    for (int i = 0; i < 16; ++i)
    {
        if (!PCB_SET_STATUS[i])
        {
            PCB_SET_STATUS[i] = 1;
            return (PCB *)((int)PCB_SET + 4096 * i);
        }
    }

    return nullptr;
}
void ProgramManager::releasePCB(PCB *program)
{
    int index = ((int)program - (int)PCB_SET) / 4096;
    PCB_SET_STATUS[index] = 0;
}
void ProgramManager::initializeTSS()
{

    int size = sizeof(TSS);
    int address = (int)&tss;

    memset((char *)address, 0, size);
    tss.ss0 = 0x10; 

    int low, high, limit;

    limit = size - 1;
    low = (address << 16) | (limit & 0xff);
    // DPL = 0
    high = (address & 0xff000000) | ((address & 0x00ff0000) >> 16) | ((limit & 0xff00) << 16) | 0x00008900;

    int selector = asm_add_global_descriptor(low, high);
    // RPL = 0
    asm_ltr(selector << 3);
    tss.ioMap = address + size;
}

int ProgramManager::executeProcess(const char *filename, int priority)
{
    bool status = irm.getInterruptStatus();
    irm.disableInterrupt();

    int pid = executeThread((ThreadFunction)load_process,(void *)filename, filename, priority);
    if (pid == -1)
    {
        if(status)
            irm.enableInterrupt();
        return -1;
    }

    PCB *process = findbyPID(pid);

    process->pageDirectoryAddress = createProcessPageDirectory();
    if (!process->pageDirectoryAddress)
    {
        process->status = 4;
        if(status)
            irm.enableInterrupt();
        return -1;
    }

    
    bool res = createUserVirtualPool(process);

    if (!res)
    {
        process->status = 4;
        if(status)
            irm.enableInterrupt();
        return -1;
    }
    table[processCount] = pid;
    process -> count = processCount;
    processCount = (processCount + 1) % 4096;
    
    if(status)
        irm.enableInterrupt();

    return pid;
}

int ProgramManager::createProcessPageDirectory()
{
    int vaddr = memoryManager.allocatePages(0, 1, 1);
    if (!vaddr)
    {
        //printf("can not create page from kernel\n");
        return 0;
    }

    memset((char *)vaddr, 0, 4096);

    int *src = (int *)(0xfffff000 + 0x300 * 4);
    int *dst = (int *)(vaddr + 0x300 * 4);
    for (int i = 0; i < 256; ++i)
    {
        dst[i] = src[i];
    }

    ((int *)vaddr)[1023] = memoryManager.vaddr2paddr(vaddr) | 0x7;

    return vaddr;
}

bool ProgramManager::createUserVirtualPool(PCB *process)
{
    int sourcesCount = (0xc0000000 - 0x8048000) / 4096;
    int bitmapLength = ceil(sourcesCount / 8.0);

    int pagesCount = ceil(bitmapLength / 4096.0);

    int start = memoryManager.allocatePages(0, pagesCount,1);

    if (!start)
    {
        return 0;
    }

    memset((char *)start, 0, 4096 * pagesCount);
    (process->userVirtual).initialize((char *)start, sourcesCount, 0x8048000);

    return 1;
}

void load_process(const char *filename)
{
    irm.disableInterrupt();

    PCB *process = programManager.running;
    myprintf("%p\n",process);
    ProcessStartStack *interruptStack =
        (ProcessStartStack *)((int)process + 4096 - sizeof(ProcessStartStack));

    interruptStack->edi = 0;
    interruptStack->esi = 0;
    interruptStack->ebp = 0;
    interruptStack->esp_dummy = 0;
    interruptStack->ebx = 0;
    interruptStack->edx = 0;
    interruptStack->ecx = 0;
    interruptStack->eax = 0;
    interruptStack->gs = 0;

    interruptStack->fs = programManager.USER_DATA_SELECTOR;
    interruptStack->es = programManager.USER_DATA_SELECTOR;
    interruptStack->ds = programManager.USER_DATA_SELECTOR;

    interruptStack->eip = (int)filename;
    interruptStack->cs = programManager.USER_CODE_SELECTOR;   
    interruptStack->eflags = (0 << 12) | (1 << 9) | (1 << 1); 

    interruptStack->esp = memoryManager.allocatePages(1, 1,1);
    if (interruptStack->esp == 0)
    {
        myprintf("can not build process!\n");
        process->status = 4;
        while(1)
            continue;
    }
    interruptStack->esp += 4096;
    interruptStack->ss = programManager.USER_STACK_SELECTOR;

    asm_start_process((int)interruptStack);
}

void ProgramManager::activateProgramPage(PCB *program)
{
    int paddr = 0x200000;

    if (program->pageDirectoryAddress)
    {
        tss.esp0 = (int)program + 4096;
        paddr = memoryManager.vaddr2paddr(program->pageDirectoryAddress);
    }

    asm_update_cr3(paddr);
}

int ProgramManager::fork()
{
    bool status = irm.getInterruptStatus();
    irm.disableInterrupt();

    PCB *parent = this->running;
    if (!parent->pageDirectoryAddress)
    {
        if(status)
            irm.enableInterrupt();
        return -1;
    }

    int pid = executeProcess("", 0);
    if (pid == -1)
    {
        if(status)
            irm.enableInterrupt();
        return -1;
    }

    PCB *child = findbyPID(pid);
    bool flag = copyProcess(parent, child);

    if (!flag)
    {
        child->status = 4;
        if(status)
            irm.enableInterrupt();
        return -1;
    }

    if(status)
        irm.enableInterrupt();
    return pid;
}

bool ProgramManager::copyProcess(PCB *parent, PCB *child)
{
    ProcessStartStack *childpss =
        (ProcessStartStack *)((int)child + 4096 - sizeof(ProcessStartStack));
    ProcessStartStack *parentpss =
        (ProcessStartStack *)((int)parent + 4096 - sizeof(ProcessStartStack));
    memcpy(childpss,parentpss , sizeof(ProcessStartStack));
    childpss->eax = 0;

    child->stack = (int *)childpss - 8;
    child->stack[0] = 0;
    child->stack[1] = 0;
    child->stack[2] = 0;
    child->stack[3] = 0;
    child->stack[4] = 0;
    child->stack[5] = (int)asm_start_process;
    child->stack[6] = 0;             
    child->stack[7] = (int)childpss; 

    child->status = 1;
    child->parentPid = parent->pid;
    child->priority = parent->priority;
    child->ticks = parent->ticks;
    child->ticksPassedBy = parent->ticksPassedBy;
    memcpy(child->name, parent->name,21);

    int bitmapLength = parent->userVirtual.getlen();
    int bitmapBytes = ceil(bitmapLength / 8.0);
    memcpy(child->userVirtual.getBitMap(), parent->userVirtual.getBitMap(), bitmapBytes);

    char *buffer = (char *)memoryManager.allocatePages(0, 1, 1);
    if (!buffer)
    {
        child->status = 4;
        return 0;
    }

    int childPageDirPaddr = memoryManager.vaddr2paddr(child->pageDirectoryAddress);
    int parentPageDirPaddr = memoryManager.vaddr2paddr(parent->pageDirectoryAddress);
    int *childPageDir = (int *)child->pageDirectoryAddress;
    int *parentPageDir = (int *)parent->pageDirectoryAddress;

    memset((void *)child->pageDirectoryAddress, 0, 768 * 4);

    for (int i = 0; i < 768; ++i)
    {

        if (!(parentPageDir[i] & 0x1))
        {
            continue;
        }

        int paddr = memoryManager.allocatePhysicalPages(1, 1);

        if(!paddr)
            paddr = memoryManager.allocPhysicalBySwap(1);
        if (paddr == -1)
        {
            child->status = 4;
            return 0;
        }
        

        int pde = parentPageDir[i];
        int *pageTableVaddr = (int *)(0xffc00000 + (i << 12));

        asm_update_cr3(childPageDirPaddr); 

        childPageDir[i] = (pde & 0x00000fff) | paddr;
        memset(pageTableVaddr, 0, 4096);

        asm_update_cr3(parentPageDirPaddr); 
    }

    for (int i = 0; i < 768; ++i)
    {

        if (!(parentPageDir[i] & 0x1))
        {
            continue;
        }

        int *pageTableVaddr = (int *)(0xffc00000 + (i << 12));

        for (int j = 0; j < 1024; ++j)
        {
            if (!(pageTableVaddr[j] & 0x1))
            {
                continue;
            }

            int paddr = memoryManager.allocatePhysicalPages(1, 1);

            if(!paddr)
                paddr = memoryManager.allocPhysicalBySwap(1);

            if (paddr == -1)
            {
                child->status = 4;
                return 0;
            }

            void *pageVaddr = (void *)((i << 22) + (j << 12));
            int pte = pageTableVaddr[j];
            memcpy(buffer,pageVaddr , 4096);

            memoryManager.pushUser((int)pageVaddr | (child->count));

            asm_update_cr3(childPageDirPaddr); 

            pageTableVaddr[j] = (pte & 0x00000fff) | paddr;
            memcpy(pageVaddr, buffer, 4096);

            asm_update_cr3(parentPageDirPaddr); 
        }
    }

    memoryManager.releasePages(0, (int)buffer, 1);
    return 1;
}
void program_exit()
{
    PCB *thread = programManager.running;
    thread->status = 4;
    if (thread->pid)
        programManager.schedule();
    else
    {
        irm.disableInterrupt();
        myprintf("halt\n");
        while(1)
            continue;
    }
}