#include "asm_utils.h"
#include "interrupt.h"
#include "os_type.h"
#include "screen.h"
#include "program.h"
#include "sync.h"
#include "random.h"

InterruptManager irm;
Screen screen;
ProgramManager programManager;
Semaphore chopsticks[5];
uint seed;
int argarr[5];

void philosopher(void *arg) 
{   int index=*(int *)arg;
    uint time1,time2;
    uint i;
    while(1)
    {   time1=100000000+10000*rand();
        time2=100000000+10000*rand();
        for(i=0;i<time1;i++)
            continue;
        myprintf("#%d:I'm hungry\n",index);
        chopsticks[index].P();
        myprintf("#%d:I've got the chopstick on the left\n",index); 
        for(i=0;i<300000000;i++)
            continue;
        chopsticks[(index+1)%5].P(); 
        myprintf("#%d:I've got the chopstick on the right\n",index); 
        for(i=0;i<time2;i++)
            continue;
        chopsticks[index].V();
        chopsticks[(index+1)%5].V(); 
        myprintf("#%d:I'm full\n",index);
    }
}
void first_thread(void *arg)
{   srand(12);
    int i;
    for(i=0;i<5;i++)
    {   chopsticks[i].initialize(1);
        argarr[i]=i;
    }    
    for(i=0;i<5;i++)
        programManager.executeThread(philosopher,(void *)(argarr+i),"philosopher",1);    
    while(1)
        continue;
}
extern "C" void setup_kernel()
{   
    asm_hello_world();
    screen.initialize();
    screen.clearScreen();
    irm.initialize();
    irm.enableTimeInterrupt();
    
    irm.setTimeInterrupt((void *)asm_time_interrupt_handler);
    
    programManager.initialize();   
   
    int pid=programManager.executeThread(first_thread, nullptr, "first_thread", 1);
    
    if (pid == -1)
    {
        myprintf("can not execute thread\n");
        while(1)
            continue;
    }
    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = (PCB *)((int)item-(int)&(((PCB *)0)->tagInGeneralList));
    firstThread->status = 2;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);
    while(1)
        continue;
}
