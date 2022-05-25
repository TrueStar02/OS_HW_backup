#include "syscall.h"
#include "interrupt.h"
#include "tools.h"
#include "asm_utils.h"
#include "os_extern.h"

int system_call_table[256];

SystemService::SystemService() {
    initialize();
}

void SystemService::initialize()
{
    memset((char *)system_call_table, 0, sizeof(int) * 256);
    irm.setInterruptDescriptor(0x80, (uint32)asm_system_call_handler, 3);
}

bool SystemService::setSystemCall(int index, int function)
{
    system_call_table[index] = function;
    return true;
}

int syscall_test(int first, int second, int third, int forth, int fifth)
{
    myprintf("systerm call test: %d, %d, %d, %d, %d\n",
           first, second, third, forth, fifth);
    return first + second + third + forth + fifth;
}