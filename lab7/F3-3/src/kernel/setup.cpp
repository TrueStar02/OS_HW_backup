#include "asm_utils.h"
#include "interrupt.h"
#include "os_type.h"
#include "screen.h"
#include "program.h"
#include "sync.h"
#include "random.h"

class Monitor
{   private:
        Condition philo[5];
        int state[5];
    public:
        void initialize(void)
        {   for(int i=0;i<5;i++)
            {   philo[i].initialize();
                state[i]=0;
            }
        }
        void test(int i)
        {   if(state[(i+4)%5]!=2&&state[i]==1&&state[(i+1)%5]!=2)
            {   state[i]=2;
                myprintf("#%d:I've got the chopsticks,I can eat now\n",i); 
                philo[i].V();
            }
        }
        void be_hungry(int i)
        {   state[i]=1;
            myprintf("#%d:I'm hungry\n",i);
            test(i);
            if(state[i]==1)
                philo[i].P();
        }
        void be_full(int i)
        {   state[i]=0;
            myprintf("#%d:I'm full\n",i);
            test((i+1)%5);
            test((i+4)%5);
        }

}monitor;

InterruptManager irm;
Screen screen;
ProgramManager programManager;
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
        monitor.be_hungry(index);
        for(i=0;i<time2;i++)
            continue;
        monitor.be_full(index);    
    }
}
void first_thread(void *arg)
{   srand(12);
    int i;
    for(i=0;i<5;i++)
        argarr[i]=i;
    monitor.initialize();
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
