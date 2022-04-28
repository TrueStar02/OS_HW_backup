#include "program.h"
#include "stdlib.h"
#include "interrupt.h"
#include "asm_utils.h"
#include "screen.h"
#include "os_extern.h"

               
char PCB_SET[65536]; 
bool PCB_SET_STATUS[16];     

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
}
int ProgramManager::executeThread(ThreadFunction function, void *parameter, const char *name, int priority)
{
    bool status = irm.getInterruptStatus();
    irm.disableInterrupt();

    
    PCB *thread = allocatePCB();

    if (!thread)
        return -1;

    
    int * ptr=(int *)thread;
    for(int i=0;i<1024;i++)
        ptr[i]=0;

    for (int i = 0; i < 20 && name[i]; ++i)
        thread->name[i] = name[i];

    thread->status = 1;
    thread->priority = priority;
    thread->ticks =  10;
    thread->ticksPassedBy = 0;
    thread->pid = ((int)thread - (int)PCB_SET) / 4096;

    thread->stack = (int *)((int)thread + 4096);
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