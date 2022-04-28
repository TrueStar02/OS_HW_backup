#ifndef SYNC_H_
#define SYNC_H_

#include "os_type.h"

class SpinLock
{   private:
        uint32 bolt;
    public:
        SpinLock();
        void initialize();
        void lock();
        void unlock();
};
#endif