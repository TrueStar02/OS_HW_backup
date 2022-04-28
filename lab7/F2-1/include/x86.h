#ifndef X86_H_
#define X86_H_
#include "os_type.h"

static inline uint8 inb(uint16 port)
{
  uint8 data;
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
  return data;
}

static inline void outb(uint16 port,uint8 data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

#endif
