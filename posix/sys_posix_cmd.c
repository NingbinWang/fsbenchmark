#include"sys_posix_cmd_priv.h"

/**@fn         sys_posix_kill
 * @brief      调用系统命令,如果命令的前缀为$，则进程在后台执行
 * @param[in]  iPid 进程ID
 * @return     无
 */
VOID sys_posix_kill(INT iPid)
{
    if(iPid <= 0)
    {
        return ;
    }
	kill(iPid, SIGKILL);
}

/**@fn         sys_posix_verify
 * @brief      判断进程是否存在
 * @param[in]  iPid 进程ID
 * @return     存在返回OK，不存在返回ERROR
 */
INT sys_posix_verify(INT iPid)
{
    INT iRet = ERROR;
    if(iPid <= 0)
    {
        return iRet;
    }
	iRet = kill(iPid, 0);
    return iRet;
}

/**@fn         sys_posix_ioctl
 * @brief      底层数据交互接口,IO控制接口
 * @param[in]  iFd      设备句柄
 * @param[in]  uCmd     命令
 * @param[out] pData    数据缓存
 * @return     存在返回OK，不存在返回ERROR
 */
INT sys_posix_ioctl(INT iFd, UINT uCmd,VOID *pData)
{
    return ioctl(iFd, uCmd, pData);
}

/**@fn         sys_posix_cmd_call
 * @brief      调用系统命令,如果命令的前缀为$，则进程在后台执行
 * @param[in]  strCmd 系统命令字符串
 * @return     调用失败，返回-1；调用成功，若后台执行返回大于0的子进程号，前台执行则返回system接口返回值
 */
INT sys_posix_cmd_call(const CHAR *strCmd)
{

    INT iRet = ERROR;
    if(NULL == strCmd)
    {
        return iRet;
    }
    iRet = system(strCmd);
    return iRet;
}


/**@fn         sys_posix_move
 * @brief      移动文件 
 * @param[in]  strSrcPath  待移动文件全路径
 * @param[in]  strDstPath  目的路径,全路径
 * @return     成功返回 0   错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_posix_move(const CHAR *strSrcPath, const CHAR *strDstPath)
{
    INT iRet = ERROR;
    CHAR strSysCmd[128] = {0};
    if(NULL == strSrcPath || NULL == strDstPath)
    {
        return iRet;
    }
    snprintf(strSysCmd,sizeof(strSysCmd) -1 ,"mv %s %s",strSrcPath,strDstPath);
    iRet = sys_posix_cmd_call(strSysCmd);
    if (OK != iRet)
    {
        printf("move file error  :%s  \n",strSysCmd);
    }
    return iRet;
}



/**@fn         sys_posix_cp
 * @brief      拷贝文件 
 * @param[in]  strSrcPath  待移动文件全路径
 * @param[in]  strDstPath  目的路径,全路径
 * @return     成功返回 0   错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_posix_cp(const CHAR *strSrcPath, const CHAR *strDstPath)
{
    INT iRet = ERROR;
    CHAR strSysCmd[128] = {0};
    if(NULL == strSrcPath || NULL == strDstPath)
    {
        return iRet;
    }
    snprintf(strSysCmd,sizeof(strSysCmd) -1 ,"cp %s %s",strSrcPath,strDstPath);
    iRet = sys_posix_cmd_call(strSysCmd);
    if (OK != iRet)
    {
        printf("move file error  :%s  \n",strSysCmd);
    }
    return iRet;
}


/**@fn         sys_posix_mkdir
 * @brief      创建目录
 * @param[in]  strPath 路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_posix_mkdir(const CHAR *strPath)
{
    INT iRet = ERROR;
    if(NULL == strPath)
    {
        return ERROR;
    }
	iRet = sys_posix_access(strPath);
    if(iRet == OK)
    {
        return OK;
    }
    iRet = mkdir(strPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (OK != iRet)
    {
        printf("strPathName:%s error \n",strPath);
    }
    return iRet;
}

/**@fn         sys_posix_rmdir
 * @brief      删除目录
 * @param[in]  strPath 路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_posix_rmdir(const CHAR *strPath)
{
    INT iRet = ERROR;	
    if(NULL == strPath)
    {
        return ERROR;
    }
	
    iRet = rmdir(strPath);
    if (OK != iRet)
    {
        printf("strPathName :%s error \n",strPath);
    }
    return iRet;
}
INT access_time[MAX_FILENUM] = {-1};
long access_count = 0;
INT access_flag = 0;
/**@fn         sys_posix_access
 * @brief      判断文件是否存在
 * @param[in]  strPath 路径
 * @return     存在返回 0 失败返回 其他
 */    
INT sys_posix_access(const CHAR *strPath)
{
    INT iRet = ERROR;
     UINT64  start,end;
    if(!strPath)
    {
        return iRet;
    }
    sys_time_get_msec(&start);
    iRet = access(strPath, F_OK);
    sys_time_get_msec(&end);
    access_time[access_count++]=(INT)(end-start);
    if(access_count == MAX_FILENUM)
    {
           access_count = 0;
           access_flag = 1;
    }
    return iRet; 
}

VOID access_bench(REPORT_LIBCAPI_T* info)
{
    int i;
    long len;
    long sum;
    if(access_flag)
        len=MAX_SYNC;
    else
        len=access_count;
    for(i = 0;i < len;i++)
    {
        sum += access_time[i];
    }
    if(len < 2)
      return;
    sys_bubble_sort(access_time,len);
    info->min = access_time[0];
    info->max = access_time[len-1];
    info->avg = sum/len;
    info->count = len;
    //printf("access min:%4dms,max:%4dms,avg:%4d\r\n",sync_time[0],sync_time[len],sum/len);
}

INT unlink_time[MAX_FILENUM] = {-1};
long unlink_count = 0;
INT unlink_flag = 0;
/**@fn         sys_posix_rm
 * @brief      删除
 * @param[in]  strPath 文件路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_ID_E
 */
INT sys_posix_rm(const CHAR *strPath)
{
    INT iRet = ERROR;
    UINT64  start,end;
    if(NULL == strPath)
    {
        return ERROR;
    } 
    iRet = sys_posix_access(strPath);
    if(iRet != OK)
    {
        return OK;
    }
    sys_time_get_msec(&start);
    iRet = unlink(strPath);
    sys_time_get_msec(&end);
    if (iRet < 0)
    {
        printf("[%s] :error:%s iRet:%d \n",__FUNCTION__,strerror(errno),iRet);
        return iRet;
    }
      unlink_time[unlink_count++]=(INT)(end-start);
    if(unlink_count == MAX_FILENUM)
    {
           unlink_count = 0;
           unlink_flag = 1;
    }
	return OK;
}

VOID unlink_bench(REPORT_LIBCAPI_T* info)
{
    int i;
    long len;
    long sum;
    if(unlink_flag)
        len=MAX_SYNC;
    else
        len=unlink_count;
    if(len < 2)
      return;
    for(i = 0;i < len;i++)
    {
        sum += unlink_time[i];
    }
    sys_bubble_sort(unlink_time,len);
    info->min = unlink_time[0];
    info->max = unlink_time[len-1];
    info->avg = sum/len;
    info->count = len;
    //printf("access min:%4dms,max:%4dms,avg:%4d\r\n",sync_time[0],sync_time[len],sum/len);
}


INT sync_time[MAX_SYNC] = {-1};
INT sync_count = 0;
INT sync_flag = 0;
/**@fn          sys_posix_sync      
 * @brief       强制写所有文件到存储介质
 * @return      成功 0  失败 -1 
 */
VOID sys_posix_sync(VOID)
{
     UINT64  start,end;
      sys_time_get_msec(&start);
    sync();
     sys_time_get_msec(&end);
    sync_time[sync_count++]=(INT)(end-start);
    if(sync_count == MAX_SYNC)
    {
           sync_count = 0;
           sync_flag = 1;
    }
}
VOID sync_bench(REPORT_LIBCAPI_T* info)
{
    int i;
    long len;
    long sum;
    if(sync_flag)
        len=MAX_SYNC;
    else
        len=sync_count;
    if(len < 2)
      return;
    for(i = 0;i < len;i++)
    {
        sum += sync_time[i];
    }
    sys_bubble_sort(sync_time,len);
    info->min = sync_time[0];
    info->max = sync_time[len-1];
    info->avg = sum/len;
    info->count = len;
    //printf("sync min:%4dms,max:%4dms,avg:%4d\r\n",sync_time[0],sync_time[len],sum/len);
}

INT rename_time[MAX_FILENUM] = {-1};
long rename_count = 0;
INT rename_flag = 0;
/**@fn         sys_posix_rename
 * @brief      重命名文件
 * @param[in]  strOldPath 旧文件路径
 * @param[in]  strNewPath 新文件路径
 * @return     成功返回 0  错误返回 其他, 参考ERROR_ID_E
 */
INT sys_posix_rename(const CHAR *strOldPath, const CHAR *strNewPath)
{
    INT iRet = ERROR;
    UINT64  start,end;
     
    if(NULL == strOldPath || NULL == strNewPath)
    {
        return ERROR;
    }
    sys_time_get_msec(&start);
    iRet = rename(strOldPath, strNewPath);
    sys_time_get_msec(&end);
    if (OK != iRet)
    {
        printf("strOldPath :%s :%s error \n",strOldPath, strNewPath);
    }
    rename_time[rename_count++]=(INT)(end-start);
    if(rename_count == MAX_FILENUM)
    {
           rename_count = 0;
           rename_flag = 1;
    }
    return iRet;
}


VOID rename_bench(REPORT_LIBCAPI_T* info)
{
    int i;
    long len = 0;
    long sum = 0;
    if(rename_flag)
        len=MAX_FILENUM;
    else
        len=rename_count;
    for(i = 0;i < len;i++)
    {
        sum += rename_time[i];
    }
    if(len < 2)
      return;
    sys_bubble_sort(rename_time,len);
    info->min = rename_time[0];
    info->max = rename_time[len-1];
    info->avg = sum/len;
    info->count = len;
    //printf("rename sum = %d len = %d min:%4dms,max:%4dms,avg:%4d\r\n",sum,len,rename_time[0],rename_time[len],sum/len);
}

/**@fn         sys_posix_fcntl
 * @brief      设置套接字属性，参考man fcntl
 * @param[in]  iFd      套接字句柄
 * @param[in]  iType    操作层面，比如是套接字层面(SOL_SOCKET)还是协议层面
 * @param[in]  iOptions 操作名称
 * @return     成功返回对应的值；失败参考ERROR_ID_E
 */
INT sys_posix_fcntl(INT iFd, UINT iType, UINT iOptions)
{
    return fcntl(iFd, ((iType == SYS_SOL_SOCKET) ? SOL_SOCKET : iType),  iOptions);
}

/**@fn         sys_posix_select
 * @brief      select封装。
 * @param[in]  iFds  , 最大iFd + 1
 * @param[in]  pStReadFdSet  , 可读集合
 * @param[in]  pStWriteFdSet  , 可写集合
 * @param[in]  pStExceptionFdSet  , 异常集合
 * @param[in]  pStTimeOut  , 超时时间
 * @return     > 0 有事件(不代表有事件的iFd个数)； = 失败返回ERROR
 * @return     = 0 无事件 @return  ERROR , 失败，通过get_last_errno获取错误码
 * @note 并非所有的系统都支持读、写、异常集合,
 *    比如安霸就只支持读集合
 */
INT sys_posix_select(INT iFds, sys_fd_set *pStReadFdSet, sys_fd_set *pStWriteFdSet,
                  sys_fd_set *pStExceptionFdSet, struct sys_timeval *pStTimeOut)
{
    INT iRet = ERROR;
    iRet = select(iFds,(fd_set*)pStReadFdSet, (fd_set *)pStWriteFdSet,(fd_set *)pStExceptionFdSet,(struct timeval *)pStTimeOut);
    return iRet;
}


/**@fn         sys_posix_get_last_errno      
 * @brief      获取套接字错误码
 * @param[in]  无
 * @return     详见errno 
 */
INT sys_posix_get_last_errno()
{
    return errno;
}


VOID sys_posix_gencsv(char* csvfilename,const CHAR *strMode)
{
    FILE* fp;
    long i = 0;
    long len = 0;
    fp =  fopen(csvfilename,strMode);
    if(fp == NULL){
        printf("create file %s fail\n",csvfilename);
    }
    //access
    if(access_flag == 1){
        len = MAX_FILENUM;
    }else{
        len = access_count;
    }
    fprintf(fp,"access:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",access_time[i]);
        }else{
            fprintf(fp,"%d,",access_time[i]);
        }
    }
    fprintf(fp,"\n");
    //unlink
    if(unlink_flag == 1){
        len = MAX_FILENUM;
    }else{
        len = unlink_count;
    }
    fprintf(fp,"unlink:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",unlink_time[i]);
        }else{
            fprintf(fp,"%d,",unlink_time[i]);
        }
    }
    fprintf(fp,"\n");
    //sync
    if(sync_flag == 1){
        len = MAX_SYNC;
    }else{
        len = sync_count;
    }
    fprintf(fp,"sync:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",sync_time[i]);
        }else{
            fprintf(fp,"%d,",sync_time[i]);
        }
    }
    fprintf(fp,"\n");
    //rename
    if(rename_flag == 1){
        len = MAX_FILENUM;
    }else{
        len = rename_count;
    }
    fprintf(fp,"rename:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",rename_time[i]);
        }else{
            fprintf(fp,"%d,",rename_time[i]);
        }
    }
    fprintf(fp,"\n");
    fclose(fp);

}