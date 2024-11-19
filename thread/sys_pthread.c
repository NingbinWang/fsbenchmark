#include"sys_pthread_priv.h"

INT (*sys_printf)(const char *format, ...) = printf;

static void *sys_thread_wrapper(void *arg)
{
    SYS_FUNC_WARP stfunc = (*(SYS_FUNC_WARP *)arg);
    free(arg);

    if(strlen(stfunc.strName))
    {
        prctl(PR_SET_NAME, stfunc.strName, 0, 0, 0);
    }

    stfunc.entry(stfunc.arg[0],stfunc.arg[1],stfunc.arg[2],stfunc.arg[3],stfunc.arg[4],
                stfunc.arg[5],stfunc.arg[6],stfunc.arg[7],stfunc.arg[8],stfunc.arg[9]);
    return NULL;
}

/**@brief  获取系统线程最大和最小优先级
 * @param  pMinPrio  最小优先级
 * @param  pMaxPrio  最大优先级
 * @return 成功返回 0
 * @return 错误返回 其他
 * @see
 * @note
 */
INT sys_pthread_priority_get(INT *pMinPrio, INT *pMaxPrio)
{
    /* get the allowable iPriority range for the scheduling policy */
    if(pMinPrio != NULL)
    {
        (*pMinPrio) = sched_get_priority_min(SCHED_RR);
        if(*pMinPrio == -1)
        {
            return -1;
        }
    }
    if(pMaxPrio != NULL)
    {
        (*pMaxPrio) = sched_get_priority_max(SCHED_RR);
        if(*pMaxPrio == -1)
        {
            return -1;
        }
    }

    return 0;
}

/**@brief  设置线程状态属性
 * @param  attr     线程属性指针
 * @param  iPriority 任务优先级
 * @param  stacksize 创建任务栈大小
 * @return 成功返回 0
 * @return 错误返回 其他
 * @see
 * @note
 */
static INT sys_pthread_set_attr(pthread_attr_t *attr, INT iPriority, size_t* stacksize)
{
    INT iRetVal;
    //struct sched_param    params;
   // INT maxPriority, minPriority;

    iRetVal = pthread_attr_init(attr);
    if(iRetVal != 0)
    {
        return iRetVal;
    }

    /* use the round robin scheduling algorithm */
    //设置线程调度模式，循环
    // iRetVal = pthread_attr_setschedpolicy(attr, SCHED_RR);
    // if(iRetVal != 0)
    // {
    //     pthread_attr_destroy(attr);
    //     return iRetVal;
    // }

    /* set the thread to be detached */
    //设置为分离线程，让线程结束的话释放自己的资源
    iRetVal = pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
    if(iRetVal != 0)
    {
        pthread_attr_destroy(attr);
        return iRetVal;
    }

    // /* first get the scheduling parameter, then set the new iPriority */
    // //设置线程的优先级
    // iRetVal = pthread_attr_getschedparam(attr, &params);
    // if(iRetVal != 0)
    // {
    //     pthread_attr_destroy(attr);
    //     return iRetVal;
    // }

    // iRetVal = sys_pthread_priority_get(&minPriority, &maxPriority);
    // if(iRetVal != 0)
    // {
    //     pthread_attr_destroy(attr);
    //     return iRetVal;
    // }
    // if(iPriority < minPriority)
    // {
    //     iPriority = minPriority;
    // }
    // else if(iPriority > maxPriority)
    // {
    //     iPriority = maxPriority;
    // }
    // params.sched_priority = iPriority;
    // iRetVal = pthread_attr_setschedparam(attr, &params);
    // if(iRetVal != 0)
    // {
    //     pthread_attr_destroy(attr);
    //     return iRetVal;
    // }

#ifdef USE_NPTL
    /* nptl创建线程默认继承父线程的调度优先级，需要设置inheritsched为
       PTHREAD_EXPLICIT_SCHED，自己设置的调度策略和优先级才能生效 */
    iRetVal = pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
    if(iRetVal != 0)
    {
        pthread_attr_destroy(attr);
        return iRetVal;
    }
#endif

    /* when set stack size, we define a minmum value to avoid fail */
    //设置栈大小，最小8192
    if(*stacksize < PTHREAD_STACK_MIN)
    {
        *stacksize = PTHREAD_STACK_MIN;
    }
    iRetVal = pthread_attr_setstacksize(attr, *stacksize);
    if(iRetVal != 0)
    {
        pthread_attr_destroy(attr);
        return iRetVal;
    }
    return 0;
}
/**@fn         sys_pthread_create
 * @brief      创建线程
 * @param[in]  pStTid      任务ID句柄指针
 * @param[in]  strTaskName 任务名，包含字符串结束符长度不超过16字节
 * @param[in]  uPriority   任务优先级 TASK_PRIORITY_E
 * @param[in]  uStackSize  创建任务栈大小
 * @param[in]  funcptr     创建任务函数
 * @param[in]  args        参数个数
 * @return     成功返回 0  错误返回 其他
 */
INT sys_pthread_create(TASK_ID *pStTid, CHAR *strTaskName, UINT uPriority, UINT uStackSize,
        FUNCPTR pFunc,
        unsigned args, ...)
{
    INT iIndex = 0;
    INT iRetVal = 0;
    void *arg[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    pthread_t *pStRaw = NULL;
    va_list   ap;
    pthread_t ptid;
    pthread_attr_t attr;    
    
    SYS_FUNC_WARP *pstFunc = NULL;
    size_t iAlignStackSize = uStackSize;
    //check param 
    if(pFunc == NULL || args > 10)
    {
        return ERROR;
    }

    va_start(ap, args);    
    for (iIndex = 0; iIndex < args; iIndex++)  
    {       
       arg[iIndex] = va_arg(ap, void *);        
       //printf("pthreadSpawn: arg[%d] = %d", iIndex, arg[iIndex]);  
    }   
    va_end(ap);
    //align 
    iAlignStackSize = TASK_STACK_ALIGN(uStackSize);
    iRetVal = sys_pthread_set_attr(&attr, uPriority, &iAlignStackSize);
    if( iRetVal != 0 )
    {
        printf("sys pthread set attr error iRetVal:%d \n",iRetVal);
        return iRetVal;
    }

    if(pStTid != NULL)	
    {       
        pStRaw = (pthread_t*)pStTid; 
    }   
    else    
    {       
        pStRaw = &ptid; 
    }
    
    /* 	 
      * If the total optional argumens is 0 or 1, we call the pthread_create directly. 	 
      * Otherwise, we wrap a start routine.	 
      */
  
    pstFunc = (SYS_FUNC_WARP *)malloc(sizeof(SYS_FUNC_WARP));
    if(pstFunc == NULL)
    {
       (void) pthread_attr_destroy(&attr);
       return ENOMEM;
    }
    memset((CHAR *)pstFunc, 0, sizeof(SYS_FUNC_WARP));
    pstFunc->entry = (FUNCPTR)pFunc;
    strncpy(pstFunc->strName,strTaskName,sizeof(pstFunc->strName) - 1);
    for (iIndex = 0; iIndex < args; iIndex++)
    {
       pstFunc->arg[iIndex] = arg[iIndex];
    }
    
    if( args == 0 )
    {
        pstFunc->arg[0]= arg[0];
    }
    
    iRetVal = pthread_create(pStRaw, &attr, sys_thread_wrapper, (void *)pstFunc);
    
    if(iRetVal != 0)
    {
       printf("sys pthread create error iRetVal:%d \n",iRetVal);
       free((CHAR *)pstFunc);
	   pthread_attr_destroy(&attr);
       return ERROR;
    }
    pthread_attr_destroy(&attr);
    //SYS_COMMON_INFO("pthread create: name:%s uPriority = %d, uStackSize = %ld \n", pstFunc->strName,uPriority, (long int)iAlignStackSize);
    return OK;
}

/**@fn         sys_pthread_join
 * @brief      等待线程结束
 * @param[in]  stTaskId   任务ID句柄
 * @return     成功返回 0  错误返回 其他
 */
INT sys_pthread_join(TASK_ID stTaskId)
{
    return pthread_join(stTaskId, NULL);
}

/**@fn         sys_pthread_cancel
 * @brief      取消线程并删除相关资源
 * @param[in]  stTaskId   任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_pthread_cancel(TASK_ID stTaskId)
{
    if(stTaskId == (TASK_ID)-1 || stTaskId == (TASK_ID)0)
    {
        return -1;
    }

    if(pthread_kill(stTaskId, 0) != 0)
    {
        printf("pthread is already unexit\n");
        return 0;
    }
/*Android cancle接口被取消*/
#ifndef ANDROID
    return pthread_cancel(stTaskId);
#else
    return OK;
#endif
}

/**@fn         sys_pthread_verify
 * @brief      验证线程是否存在
 * @param[in]  pStTid     任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_pthread_verify(TASK_ID *pStTid)
{
    if(*pStTid == (TASK_ID)-1 || *pStTid == (TASK_ID)0)
    {
        return -1;
    }

    return pthread_kill(*pStTid, 0);
}
/**@fn         sys_pthread_suspend
 * @brief      阻塞线程
 * @param[in]  pStTid        任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_pthread_suspend(TASK_ID *pStTid)
{
    return pthread_kill(*pStTid, SIGSTOP);
}

/**@fn         sys_pthread_self
 * @brief      获取调用线程的ID
 * @return     成功返回 0  返回线程对象ID
 */
TASK_ID sys_pthread_self(VOID)
{
    return pthread_self();
}

/**@fn         sys_pthread_reume
 * @brief      恢复阻塞的线程
 * @param[in]  pStTid      任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_pthread_reume(TASK_ID *pStTid)
{
    return pthread_kill(*pStTid, SIGCONT);
}

/**@fn         sys_pthread_name
 * @brief      恢复阻塞的线程
 * @param[in]  pStTid      任务ID句柄
 * @param[out] strName     线程任务名称
 * @param[in]  uNameLen    线程名称还清楚大小
 * @return     成功返回0,错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_pthread_get_name(CHAR *strName, UINT16 uNameLen)
{
    if(NULL == strName)
    {
        return ERROR;
    }
    prctl(PR_GET_NAME, (unsigned long)strName);
    return OK;
}