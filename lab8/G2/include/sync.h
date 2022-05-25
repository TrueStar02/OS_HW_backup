#ifndef SYNC_H_
#define SYNC_H_

#include "os_type.h"
#include "list.h"

class SpinLock
{   private:
        uint32 bolt;
    public:
        SpinLock();
        void initialize();
        void lock();
        void unlock();
};
class Semaphore
{   private:
        uint32 counter;
        List waiting;
        SpinLock semLock;
    public:
        Semaphore();
        void initialize(uint32 counter);
        void P();
        void V();
};
class Condition
{   private:
        uint32 counter;
        List waiting;
        SpinLock semLock;
    public:
        Condition();
        void initialize();
        void P();
        void V();
};
#endif