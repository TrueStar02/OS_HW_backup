#ifndef OS_EXTERN_H_
#define OS_EXTERN_H_
#include "interrupt.h"
#include "screen.h"
#include "program.h"
#include "memory.h"
extern InterruptManager irm;
extern Screen screen;
extern ProgramManager programManager;
extern uint seed;
extern MemoryManager memoryManager;
#endif
