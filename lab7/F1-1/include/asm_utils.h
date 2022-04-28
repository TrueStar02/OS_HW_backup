#ifndef ASM_UTILS_H
#define ASM_UTILS_H
#include "os_type.h"

extern "C" void asm_hello_world();
extern "C" void asm_lidt(uint32 start, uint16 limit);
extern "C" void default_interrupt_handler();
extern "C" void asm_time_interrupt_handler();
extern "C" void asm_switch_thread(void *cur, void *next);
extern "C" void asm_atomic_exchange(uint * key_addr,uint * bolt_addr);
#endif
