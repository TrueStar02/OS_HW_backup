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

static inline uint16 inw(uint16 port)
{
  uint16 data;
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
  return data;
}

static inline void outw(uint16 port,uint16 data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void insw(uint port, void *addr, uint cnt)
{
  asm volatile("cld; rep insw" :
               "=D" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "memory", "cc");
}
#endif
