#ifndef PROGRAM_H_
#define PROGRAM_H_

#include "list.h"
#include "addrpool.h"

typedef void (*ThreadFunction)(void *);

struct TSS
{
    int backlink;
    int esp0;
    int ss0;
    int esp1;
    int ss1;
    int esp2;
    int ss2;
    int cr3;
    int eip;
    int eflags;
    int eax;
    int ecx;
    int edx;
    int ebx;
    int esp;
    int ebp;
    int esi;
    int edi;
    int es;
    int cs;
    int ss;
    int ds;
    int fs;
    int gs;
    int ldt;
    int trace;
    int ioMap;
};

struct ProcessStartStack
{
    int edi;
    int esi;
    int ebp;
    int esp_dummy;
    int ebx;
    int edx;
    int ecx;
    int eax;
    
    int gs;
    int fs;
    int es;
    int ds;

    int eip;
    int cs;
    int eflags;
    int esp;
    int ss;
};

struct PCB
{
    int * stack;                      
    char name[21]; 
    int status;       
    int priority;                    
    int pid;   
    int count;                      
    int ticks;                       
    int ticksPassedBy;               
    ListItem tagInGeneralList;       
    ListItem tagInAllList;  
    int pageDirectoryAddress; 
    AddressPool userVirtual;      
};

class ProgramManager
{
    public:
        List allPrograms;   
        List readyPrograms; 
        PCB *running;  
        int USER_CODE_SELECTOR;  
        int USER_DATA_SELECTOR;  
        int USER_STACK_SELECTOR;   
        ProgramManager();
        void initialize();
        int executeThread(ThreadFunction function, void *parameter, const char *name, int priority);
        PCB * allocatePCB();
        void releasePCB(PCB *program);
        void schedule();
        void WakeUp(PCB * program);
        void initializeTSS();
        int executeProcess(const char *filename, int priority);   
        int createProcessPageDirectory();
        bool createUserVirtualPool(PCB *process);
        void activateProgramPage(PCB *program);
        PCB * findbyPID(int pid);
        int queryByCount(int c);
};

void program_exit();
void load_process(const char *filename);

#endif