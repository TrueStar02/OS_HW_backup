#ifndef TOOLS_H_
#define TOOLS_H_
#include "os_type.h"

void srand(uint s);
uint rand(void);
void memset(void * start,uint8 val,uint32 len);
int floor(double x);
int ceil(double x);
#endif