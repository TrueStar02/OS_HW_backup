#ifndef PROGRAM_H_
#define PROGRAM_H_

#include "list.h"

typedef void (*ThreadFunction)(void *);

struct PCB
{
    int * stack;                      
    char name[21]; 
    int status;       
    int priority;                    
    int pid;                         
    int ticks;                       
    int ticksPassedBy;               
    ListItem tagInGeneralList;       
    ListItem tagInAllList;           
};

class ProgramManager
{
    public:
        List allPrograms;   
        List readyPrograms; 
        PCB *running;       
        ProgramManager();
        void initialize();
        int executeThread(ThreadFunction function, void *parameter, const char *name, int priority);
        PCB * allocatePCB();
        void releasePCB(PCB *program);
        void schedule();
        void WakeUp(PCB * program);
};

void program_exit();


#endif