#ifndef ASM_UTILS_H
#define ASM_UTILS_H
#include "os_type.h"

extern "C" void asm_hello_world();
extern "C" void asm_lidt(uint32 start, uint16 limit);
extern "C" void default_interrupt_handler();
extern "C" void asm_time_interrupt_handler();
extern "C" void asm_switch_thread(void *cur, void *next);
extern "C" void asm_atomic_exchange(uint * key_addr,uint * bolt_addr);
extern "C" void asm_lock(uint * bolt_addr);
extern "C" void asm_init_page_reg(int *directory);
extern "C" void asm_pagefault_handler();
extern "C" int asm_system_call(int index, int first = 0, int second = 0, int third = 0, int forth = 0, int fifth = 0);
extern "C" int asm_system_call_handler();
#endif
