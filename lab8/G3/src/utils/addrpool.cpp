#include "addrpool.h"

AddressPool::AddressPool(){}

void AddressPool::initialize(char *bitmap, const int length, const int startAddress)
{
    resources.initialize(bitmap, length);
    this->startAddress = startAddress;
}

int AddressPool::allocate(const int count)
{
    int start = resources.allocate(count);
    return (start == -1) ? -1 : (start * 4096 + startAddress);
}

void AddressPool::release(const int address, const int amount)
{
    resources.release((address - startAddress) / 4096, amount);
}