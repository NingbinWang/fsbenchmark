#include "sys_libc_priv.h"
INT fopen_time[MAX_FOPEN] = {0};
long fopen_count = 0;
INT fopen_flag = 0;
/**@fn        sys_libc_fopen    
 * @brief     打开文件      
 * @param[in] strPath 文件路径
 * @param[in] mode 打开文件的方式, 参考man 3 fopen,如wr+
 * @return    成功返回 文件句柄 错误返回 NULL, 支持通过get_last_errno获取错误码
 */
FILE_ID* sys_libc_fopen(const CHAR *strPath, const CHAR *strMode)
{
    FILE *pHandle = NULL ;
    UINT64  start,end;
    if(NULL == strPath || NULL == strMode)
    {
        printf("file invaild param \n");
        return NULL;
    }
    sys_time_get_msec(&start);
    pHandle = fopen(strPath, strMode);
    sys_time_get_msec(&end);
    if (NULL == pHandle)
    {
        return NULL;
    }
    fopen_time[fopen_count++]=(INT)(end-start);
    if(fopen_count == MAX_FOPEN)
    {
        fopen_flag = 1;
        fopen_count = 0;
    }
	return (FILE_ID *)pHandle;
}


VOID fopen_bench(REPORT_LIBCAPI_T* info)
{
    int i;
    long len;
    long sum=0;
    if(fopen_flag)
        len=MAX_FOPEN;
    else
        len=fopen_count;
    if(len < 2)
      return;
    for(i = 0;i < len;i++)
    {
        sum += fopen_time[i];
    }

    sys_bubble_sort(fopen_time,len);
    info->min = fopen_time[0];
    info->max = fopen_time[len-1];
    info->avg = sum/len;
    info->count = len;
    //printf("fopen min:%4dms,max:%4dms,avg:%4d\r\n",fopen_time[0],fopen_time[len],sum/len);
}


INT fclose_time[MAX_FCLOSE] = {0};
long fclose_count = 0;
INT fclose_flag = 0;
/**@fn         sys_libc_fclose
 * @brief      关闭文件      
 * @param[in]  pFileID     文件句柄，通过sys_file_fopen获得
 * @return     成功返回0  错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_libc_fclose(FILE_ID *pFileID)
{
    INT iRet = ERROR;
    UINT64  start,end;
    if (NULL == pFileID)
    {
        printf("file invaild param \n");
        return ERROR;
    }
    sys_time_get_msec(&start);
    iRet = fclose((FILE *)pFileID);            /*关闭打开的文件*/
    sys_time_get_msec(&end);
    if (OK != iRet)
    {
        printf("fclose file error  \n");
        return ERROR;
    }
    fclose_time[fclose_count++]=(INT)(end-start);
    if(fclose_count == MAX_FCLOSE)
    {
        fclose_flag = 1;
        fclose_count = 0;
    }
	return iRet;
}
VOID fclose_bench(REPORT_LIBCAPI_T* info)
{
    int i;
    long len;
    long sum = 0;

    if(fclose_flag)
        len=MAX_FCLOSE;
    else
        len=fclose_count;
    if(len < 2)
      return;
    for(i = 0;i < len;i++)
    {
        sum += fclose_time[i];
    }
    sys_bubble_sort(fclose_time,len);
    info->min = fclose_time[0];
    info->max = fclose_time[len-1];
    info->avg = sum/len;
     info->count = len;
    //printf("fclose min:%4dms,max:%4dms,avg:%4d\r\n",fclose_time[0],fclose_time[len],sum/len);
}

/**@fn         sys_file_feof
 * @brief      判断是否已经到达文件末尾
 * @param[in]  pFileID  文件句柄
 * @return     > 0 ,已经到达文件末尾;= 0 , 未到达文件末尾;< 0 , 文件句柄错误
 */
INT sys_file_feof(FILE_ID *pFileID)
{
    if (NULL == pFileID)
    {
        return ERROR;
    }
    
	return feof((FILE *)pFileID);
}

INT fread_time[MAX_FREAD] = {0};
long fread_count = 0;
INT fread_flag = 0;

/**@fn         sys_libc_fread
 * @brief      读文件数据      
 * @param[in]  pFileID 文件句柄，通过sys_file_fopen获得
 * @param[in]  pBuffer 目的地址
 * @param[in]  uSize  每块字节数
 * @param[in]  uCount  要读取的块数
 * @return     成功返回 实际读取的块数 错误返回 ERROR, 支持通过get_last_errno获取错误码
 * @note       到达文件末尾的时候，返回值可能小于uCount
 */
INT sys_libc_fread(FILE_ID *pFileID, VOID *pBuffer, UINT uSize, UINT uCount)
{
    UINT  uReadCount= 0;
     UINT64  start,end;
    if(NULL == pFileID || NULL == pBuffer || 0 == (uSize * uCount))
    {
        printf("file invaild param :%p :%p :%d \n",pFileID,pBuffer,(uSize * uCount));
        return ERROR;
    }
    sys_time_get_msec(&start);
    uReadCount = fread(pBuffer, uSize, uCount, (FILE *)pFileID);
    sys_time_get_msec(&end);
    fread_time[fread_count++]=(INT)(end-start);
    if(fread_count == MAX_FREAD)
    {
           fread_flag = 1;
           fread_count = 0;
    }
    if (uReadCount == uCount)
    {
        
        return uReadCount;
    }
    if (sys_file_feof(pFileID) > 0)
    {
        return uReadCount;
    }
    printf("sys_feof error \n");
    return ERROR;
}
VOID fread_bench(REPORT_LIBCAPI_T* info)
{
    int i;
    long len;
    long sum=0;
    if(fread_flag)
        len=MAX_FREAD;
    else
        len=fread_count;
    if(len < 2)
      return;
    for(i = 0;i < len;i++)
    {
        sum += fread_time[i];
    }
    sys_bubble_sort(fread_time,len);
    info->min = fread_time[0];
    info->max = fread_time[len-1];
    info->avg = sum/len;
    info->count = len;
    //printf("fread min:%4dms,max:%4dms,avg:%4d\r\n",fread_time[0],fread_time[len],sum/len);
}

INT fwrite_time[MAX_FWRITE] = {0};
long fwrite_count = 0;
INT fwrite_flag = 0;

/**@fn         sys_libc_fwrite
 * @brief      写文件     
 * @param[in]  pFileID 文件句柄 
 * @param[in]  pBuffer 要写入文件的内容存放的地址
 * @param[in]  uSize   每块数据的大小
 * @param[in]  uCount  要写入的块数
 * @return     成功返回 实际写入的块数 错误返回 ERROR ,  支持通过get_last_errno获取错误码
 */
INT sys_libc_fwrite(FILE_ID *pFileID, const VOID *pBuffer, UINT uSize, UINT uCount)
{
     UINT64  start,end;
     if(NULL == pFileID || NULL == pBuffer || 0 == (uSize * uCount))
    {
        printf("file invaild param :%p  :%p :%d \n",pFileID ,pBuffer,(uSize * uCount));
        return ERROR;
    }
    sys_time_get_msec(&start);
    if (fwrite(pBuffer, uSize, uCount, (FILE *)pFileID) != uCount)
    {
        printf("fwrite error \n");
        return ERROR;
    }
    sys_time_get_msec(&end);
    fwrite_time[fwrite_count++]=(INT)(end-start);
    if(fwrite_count == MAX_FWRITE)
    {
           fwrite_flag = 1;
           fwrite_count = 0;
    }
    return uCount;    
}
VOID fwrite_bench(REPORT_LIBCAPI_T* info)
{
    int i;
    long len;
    long sum=0;
    if(fwrite_flag)
        len=MAX_FWRITE;
    else
        len=fwrite_count;
    if(len < 2)
      return;
    for(i = 0;i < len;i++)
    {
        sum += fwrite_time[i];
    }
    sys_bubble_sort(fwrite_time,len);
    info->min = fwrite_time[0];
    info->max = fwrite_time[len-1];
    info->avg = sum/len;
     info->count = len;
    //printf("fwrite min:%4dms,max:%4dms,avg:%4d\r\n",fwrite_time[0],fwrite_time[len],sum/len);
}


INT fflush_time[MAX_FSYNC] = {0};
INT fileno_time[MAX_FSYNC] = {0};
INT fsync_time[MAX_FSYNC] = {0};
long fsync_count = 0;
INT fsync_flag = 0;

/**@fn         sys_file_fsync
 * @brief      强制写文件到存储介质      
 * @param[in]  pFileID  文件句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_file_fsync(FILE_ID *pFileID)
{
    INT iRet = ERROR;
    INT iFd = -1 ;
    UINT64  start,end;
    if (NULL == pFileID)
    {
        return ERROR;
    }
    sys_time_get_msec(&start);
    iRet = fflush((FILE*)pFileID);
    sys_time_get_msec(&end);
    if (OK != iRet)
    {
        printf(" fflush error :%s iRet:%d\n",strerror(errno), iRet);
    }
    fflush_time[fsync_count]=(INT)(end-start);
    sys_time_get_msec(&start);
    iFd = fileno((FILE*)pFileID);
    sys_time_get_msec(&end);
    fileno_time[fsync_count]=(INT)(end-start);
    sys_time_get_msec(&start);
    iRet = fsync(iFd);
    sys_time_get_msec(&end);
    if (OK != iRet)
    {
        printf(" fsync error :%s iRet:%d\n",strerror(errno), iRet);
    }
    fsync_time[fsync_count++]=(INT)(end-start);
     if(fsync_count == MAX_FSYNC)
    {
           fsync_flag = 1;
           fsync_count = 0;
    }
	return iRet;
}

VOID fsync_bench(REPORT_SYNC_T* info)
{
    int i;
    long len;
    long sum=0;
    if(fsync_flag)
        len=MAX_FSYNC;
    else
        len=fsync_count;
    if(len < 2)
      return;
    for(i = 0;i < len;i++)
    {
        sum += fflush_time[i];
    }
    info->count = len;
    sys_bubble_sort(fflush_time,len);
    info->fflush_min=fflush_time[0];
    info->fflush_max=fflush_time[len-1];
    info->fflush_avg=sum/len;
    //printf("fflush_time min:%4dms,max:%4dms,avg:%4d\r\n",fread_time[0],fread_time[len],sum/len);
    sum = 0;
    for(i = 0;i < len;i++)
    {
        sum += fileno_time[i];
    }
    sys_bubble_sort(fileno_time,len);
    info->fileno_min=fileno_time[0];
    info->fileno_max=fileno_time[len-1];
    info->fileno_avg=sum/len;
    //printf("fileno_time min:%4dms,max:%4dms,avg:%4d\r\n",fread_time[0],fread_time[len],sum/len);
    sum = 0;
    sys_bubble_sort(fsync_time,len);
    for(i = 0;i < len;i++)
    {
        sum += fsync_time[i];
    }
    info->fsync_min=fsync_time[0];
    info->fsync_max=fsync_time[len-1];
    info->fsync_avg=sum/len;
    //printf("fsync_time min:%4dms,max:%4dms,avg:%4d\r\n",fread_time[0],fread_time[len],sum/len);
}


/**@fn         sys_file_fseek
 * @brief      重新定位文件读写位置
 * @param[in]  pFileID  文件句柄
 * @param[in]  lOffset为偏移量，正数表示正向偏移，负数表示负向偏移
 * @param[in]  iStartPos 设定从文件的哪里开始偏移,可能取值为：SEEK_CUR、 SEEK_END 或 SEEK_SET
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_libc_fseek(FILE_ID *pFileID, LONG lOffset, INT iStartPos)
{
    INT iRet = ERROR;
    if (NULL == pFileID)
    {
        return ERROR;
    }
    iRet = fseek((FILE *)pFileID, lOffset, iStartPos);
    if (OK != iRet)
    {
        printf("fseek error \n");
    }
    return iRet;
}

/**@fn         sys_file_ftell    
 * @brief      文件指针所指位置相对于开头的偏移，单位字节byte          
 * @param[in]  pFileID  文件句柄
 * @return     成功返回 偏移值 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
LONG sys_libc_ftell(FILE_ID *pFileID)
{
    INT iRet = ERROR;

    if (NULL == pFileID)
    {
       printf("file invaild param \n");
        return ERROR;
    }

    iRet = (LONG)ftell((FILE *)pFileID);
    if (ERROR == iRet)
    {
        printf("ftell error \n");
    }
	return iRet;
}
INT fstat_time[MAX_FSTAT] = {0};
long fstat_count = 0;
INT fstat_flag = 0;
/**@fn         sys_file_fstat    
 * @brief      查看文件信息
 * @param[in]  strPath 路径
 * @param[in]  pStSate 文件信息
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_file_fstat(const CHAR *strPath, SYS_FS_STAT_T *pStSate)
{
    INT iRet = ERROR;
    UINT64  start,end;
    struct stat stState = {0};
    if(NULL == strPath || NULL == pStSate)
    {
        return ERROR;
    }
    sys_time_get_msec(&start);
    iRet = stat(strPath, &stState);
    sys_time_get_msec(&end);
    if(iRet != OK)
    {
        printf("stat error %4dms\n\r",(INT)(end-start));
        fprintf(stderr,"[%s]strPath:%s error:%s iRet:%d \n",strPath,__FUNCTION__,strerror(errno),iRet);
        return iRet;
    }
    fstat_time[fstat_count++]=(INT)(end-start);
    if(fstat_count == MAX_FSTAT)
    {
           fstat_flag = 1;
           fstat_count = 0;
    }
    
    if (S_ISDIR(stState.st_mode))
    {
        pStSate->uAttr |= SYS_FS_ATTR_DIR;
    }

    pStSate->uSpaceSize = stState.st_blocks * 512;  /* block大小是512 字节，参考man 2 stat */
    pStSate->uSize = stState.st_size;
    pStSate->uClusterSize = stState.st_blksize;
    return iRet;
}
VOID fstat_bench(REPORT_LIBCAPI_T* info)
{
    int i;
    long len;
    long sum = 0;
    if(fstat_flag)
        len=MAX_FSTAT;
    else
        len=fstat_count;
    if(len < 2)
      return;
    for(i = 0;i < len;i++)
    {
        sum += fstat_time[i];
    }
    sys_bubble_sort(fstat_time,len);
    info->min = fstat_time[0];
    info->max = fstat_time[len-1];
    info->avg = sum/len;
     info->count = len;
    //printf("fstat min:%4dms,max:%4dms,avg:%4d\r\n",fstat_time[0],fstat_time[len],sum/len);
}




/**@fn         sys_file_read_dir 暂时考虑到拷贝封装的性能消耗，互调反馈      
 * @brief      读目录文件夹
 * @param[in]  strDir          查看目录
 * @param[in]  pUserParam      自定义参数
 * @param[in]  pCallBack       读数据回调接口
 * @return     成功 0  失败 -1 
 */
INT sys_file_read_dir(const CHAR *strDir, VOID *pUserParam, ReadDirCallBack pCallBack)
{
    INT iRet = ERROR;
    DIR *pDir = NULL ;
    struct dirent *pDirRent = NULL;
    CHAR strDirPath[512] = {0};
    if(!strDir || !pUserParam || !pCallBack)
    {
        printf("invaild param :%p %p %p\n",strDir,pUserParam,pCallBack);
        return iRet;
    }
    pDir = opendir(strDir);
    if(pDir == NULL)
    {
        printf("open dir: %s failed.\n",strDir);
        return iRet;
    }
    while((pDirRent = readdir(pDir)) != NULL)
    {
        //exfata 读不出文件
        if(strcmp(pDirRent->d_name,".") == 0)
        {
            continue;
        }
        if(strcmp(pDirRent->d_name,"..") == 0)
        {
            continue;
        }
        memset(strDirPath,0,sizeof(strDirPath));
        snprintf(strDirPath,sizeof(strDirPath),"%s/%s",strDir,pDirRent->d_name);
        pCallBack(strDirPath,pUserParam);
    }
    closedir(pDir);
    return OK;
}


/**@fn         sys_file_read_dir_file      
 * @brief      读目录文件
 * @param[in]  strDir          查看目录
 * @param[in]  pUserParam      自定义参数
 * @param[in]  pCallBack       读数据回调接口
 * @return     成功 0  失败 -1 
 */
INT sys_file_read_dir_file(const CHAR *strDir, VOID *pUserParam, ReadFileCallBack pCallBack,REPORT_FSINFO_T *fsinfo)
{
    INT iRet = ERROR;
    DIR *pDir = NULL ;
    struct dirent *pDirRent = NULL;
    UINT64  start,end;
    CHAR strFilePath[512] = {0};
    SYS_FS_STAT_T stStatBuf = {0};
    if(!strDir || !pUserParam || !pCallBack)
    {
        return iRet;
    }
    sys_time_get_msec(&start);
    pDir = opendir(strDir);
    sys_time_get_msec(&end);
    if(pDir == NULL)
    {
        printf("open dir: %s failed.\n",strDir);
        return iRet;
    }
    fsinfo->opendir_time=(int)(end-start);
    sys_time_get_msec(&start);
    while((pDirRent = readdir(pDir)) != NULL)
    {
		if(strcmp(pDirRent->d_name,".") == 0)
        {
            continue;
        }
        if(strcmp(pDirRent->d_name,"..") == 0)
        {
            continue;
        }
        snprintf(strFilePath,sizeof(strFilePath),"%s/%s",strDir,pDirRent->d_name);
        iRet = sys_file_fstat(strFilePath, &stStatBuf);
        if(iRet < 0)
        {
            printf("lstat %s error.\n", strFilePath);
            break;
        }
        pCallBack(strFilePath,&stStatBuf,pUserParam);
    }
     sys_time_get_msec(&end);
     fsinfo->readdir_time=(int)(end-start);
     closedir(pDir);
    return OK;
}


/**@brief	  获取文件当前定位
 * @param[in] pFileID  文件句柄
 * @param[in] plOffset为偏移量指针
 * @return 成功返回 0
 * @return 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_file_fgetpos(FILE_ID *pFileID, LONG *plOffset)
{
    INT iRet = ERROR;
#ifndef SYS_ANDRIOD
    if (NULL == pFileID || NULL == plOffset)
    {
        return ERROR;
    }
    
    iRet = fgetpos((FILE *)pFileID, (fpos_t *)plOffset);
    if (OK != iRet)
    {
        printf("fgetpos error \n");
    }
#endif
    return iRet;
}

/*
typedef struct
{   
    REPORT_LIBCAPI_T          fopen_report;
    REPORT_LIBCAPI_T          fclose_report;
    REPORT_LIBCAPI_T          fread_report;
    REPORT_LIBCAPI_T          fwrite_report;
    REPORT_SYNC_T             fsync_report;
    REPORT_LIBCAPI_T          sync_report;
    REPORT_LIBCAPI_T          fstat_report;
    REPORT_LIBCAPI_T          access_report;
    REPORT_LIBCAPI_T          unlink_report;
    REPORT_LIBCAPI_T          rename_report;
    REPORT_FSINFO_T           fsinfo_report;
}BENCH_REPORT_T;
*/

VOID sys_libc_gencsv(const CHAR *csvfilename, const CHAR *strMode)
{
    FILE* fp;
    long i = 0;
    long len = 0;
    fp =  fopen(csvfilename,strMode);
    if(fp == NULL){
        printf("create file %s fail\n",csvfilename);
    }
    if(fopen_flag == 1){
        len = MAX_FOPEN;
    }else{
        len = fopen_count;
    }
    //fopen
    fprintf(fp,"fopen:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",fopen_time[i]);
        }else{
            fprintf(fp,"%d,",fopen_time[i]);
        }
    }
   
    fprintf(fp,"\n");
    //fread
    if(fread_flag == 1){
        len = MAX_FREAD;
    }else{
        len = fread_count;
    }
    fprintf(fp,"fread:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",fread_time[i]);
        }else{
            fprintf(fp,"%d,",fread_time[i]);
        }
    }
    fprintf(fp,"\n");
   
    //fwrite
    if(fwrite_flag == 1){
        len = MAX_FWRITE;
    }else{
        len = fwrite_count;
    }
    fprintf(fp,"fwrite:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",fwrite_time[i]);
        }else{
            fprintf(fp,"%d,",fwrite_time[i]);
        }
    }
    fprintf(fp,"\n");
    //fsync_report
    if(fsync_flag == 1){
        len = MAX_FSYNC;
    }else{
        len = fsync_count;
    }
    fprintf(fp,"fflush:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",fflush_time[i]);
        }else{
            fprintf(fp,"%d,",fflush_time[i]);
        }
    }
    fprintf(fp,"\n");
    fprintf(fp,"fileno:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",fileno_time[i]);
        }else{
            fprintf(fp,"%d,",fileno_time[i]);
        }
    }
    fprintf(fp,"\n");
    fprintf(fp,"fsync:,");
    for(i = 0 ; i < len;i++)
    {
         if(i == len-1){
            fprintf(fp,"%d",fsync_time[i]);
        }else{
            fprintf(fp,"%d,",fsync_time[i]);
        }
    }
    fprintf(fp,"\n");
    //fclose
    if(fclose_flag == 1){
        len = MAX_FCLOSE;
    }else{
        len = fclose_count;
    }
    fprintf(fp,"fclose:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",fclose_time[i]);
        }else{
            fprintf(fp,"%d,",fclose_time[i]);
        }
    }
    fprintf(fp,"\n");
    //fstat
    if(fstat_flag == 1){
        len = MAX_FSTAT;
    }else{
        len = fstat_count;
    }
    fprintf(fp,"fstat:,");
    for(i = 0 ; i < len;i++)
    {
        if(i == len-1){
            fprintf(fp,"%d",fstat_time[i]);
        }else{
            fprintf(fp,"%d,",fstat_time[i]);
        }
    }
    fprintf(fp,"\n");
    fclose(fp);
}