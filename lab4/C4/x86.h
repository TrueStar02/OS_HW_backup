typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

static inline uchar inb(ushort port)
{
  uchar data;
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
  return data;
}

static inline void insw(uint port, void *addr, uint cnt)
{
  asm volatile("cld; rep insw" :
               "=D" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "memory", "cc");
}

static inline void outb(ushort port, uchar data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}