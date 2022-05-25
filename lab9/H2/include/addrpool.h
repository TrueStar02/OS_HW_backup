#ifndef ADDRESS_POOL_H
#define ADDRESS_POOL_H

#include "bitmap.h"
#include "os_type.h"

class AddressPool
{
    private:
        BitMap resources;
        int startAddress;

    public:
        AddressPool();
        void initialize(char *bitmap, const int length, const int startAddress);
        int allocate(const int count);
        void release(const int address, const int amount);
};

#endif