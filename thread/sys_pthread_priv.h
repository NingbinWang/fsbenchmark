#ifndef _SYS_THREAD_PRIV_H_
#define _SYS_THREAD_PRIV_H_
#include <pthread.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/prctl.h>
#include <signal.h>
#include <limits.h>
#include"sys_common.h"
#include"sys_pthread.h"
#include"utils.h"

#define	STACK_ALIGN_BYTES	(64)	 
#define MAX_THREAD_NAME_LEN (16)
#define MAX_MQUEUE_NAME_LEN (64)
#define NSEC2SEC            (1000000*1000)
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN (SIZE_8KB)
#endif

#define TASK_STACK_ALIGN(p)	BYTES_ALIGN((p), STACK_ALIGN_BYTES)

/* function wrapper structure */
typedef struct
{
	FUNCPTR entry; /* execution entry point address for thread */
    char  strName[32];
	void  *arg[10]; /* arguments */
}SYS_FUNC_WARP;

#endif