#ifndef _UTILS_H_
#define _UTILS_H_
#include "sys_common.h"
#define    BYTES_ALIGN(p, alignbyte)    (((ULONG)(p) + (ULONG)((alignbyte) - 1)) &~ ((alignbyte) - 1))
VOID sys_bubble_sort(INT arr[], INT len);


#endif