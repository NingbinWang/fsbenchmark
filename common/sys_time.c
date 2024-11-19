#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <sys/time.h> 
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include "sys_time.h"

/**@fn         sys_time_get_msec      
 * @brief      获取系统启动后运行的毫秒数
 * @param[out] uMSecond 毫秒数指针
 * @return     成功返回 OK 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT sys_time_get_msec(UINT64 *puMSecond)
{
    INT iRet = ERROR;
    struct timeval stTimeRun = {0};
    long seconds,useconds;
    //struct timespec stTimeRun = {0};
    memset(&stTimeRun,0,sizeof(stTimeRun));
    if(NULL == puMSecond)
    {
        printf("sys_time_get_msec puMSecond is null\n");
        return iRet;
    }
    iRet = gettimeofday(&stTimeRun,NULL);
    if(iRet < 0)
    {
         printf("sys_time_get_msec puMSecond clock_gettime fail\n");
        return iRet;
    }
    *puMSecond  = stTimeRun.tv_sec * 1000 + stTimeRun.tv_usec/1000;
    return iRet;      
}

