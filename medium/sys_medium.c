
#include"sys_medium_priv.h"

/**@fn         sys_medium_fs_type_convert_data  
 * @brief      将文件系统类型转换对于的参数内容
 * @param[in]  eFsType         文件系统类型  
 * @param[in]  strFileSystem   文件系统字符串  
 * @param[in]  uSize           字符串缓冲区大小
 * @return 成果返回OK，失败返回ERROR,
 */
INT sys_medium_fs_type_convert_data(SYS_MEDIUM_FS_TYPE_E eFsType,char *strData,UINT16 uSize)
{
    INT iRet = ERROR;
    if(!strData)
    {
        return iRet;
    }
    switch(eFsType)
    {
        case SYS_MEDIUM_FS_VFAT:
            /* code */
            strncpy(strData,"shortname=mixed,errors=continue,fmask=0077",uSize);
            break;
        case SYS_MEDIUM_FS_EXT4:
            strncpy(strData,"data=ordered",uSize);
            break;
        case SYS_MEDIUM_FS_EXFAT:
            /* code */
            strncpy(strData,"iocharset=cp936,fmask=0077",uSize);
            break;
        case SYS_MEDIUM_FS_NTFS:
            /* do nothing */
            break;
        default:
            printf("no support type :%d \n",eFsType);
            return iRet;
    }

    return OK;
}

/**@fn         sys_medium_fs_type_convert_string  
 * @brief      将文件系统转换对应的挂载参数
 * @param[in]  eFsType         文件系统类型  
 * @param[in]  strFileSystem   文件系统字符串  
 * @param[in]  uSize           字符串缓冲区大小
 * @return 成果返回OK，失败返回ERROR,
 */
INT sys_medium_fs_type_convert_string(SYS_MEDIUM_FS_TYPE_E eFsType,char *strFileSystem,UINT16 uSize)
{   
    if(!strFileSystem)
    {
        return -1;
    }

    switch(eFsType)
    {
        case SYS_MEDIUM_FS_VFAT:
            /* code */
            strncpy(strFileSystem,"vfat",uSize);
            break;
        case SYS_MEDIUM_FS_XFS:
            strncpy(strFileSystem,"xfs",uSize);
            break;
        case SYS_MEDIUM_FS_EXT:
            strncpy(strFileSystem,"ext",uSize);
            break;
        case SYS_MEDIUM_FS_EXT2:
            strncpy(strFileSystem,"ext2",uSize);
            break;
        case SYS_MEDIUM_FS_EXT3:
            strncpy(strFileSystem,"ext3",uSize);
            break;
        case SYS_MEDIUM_FS_EXT4:
            strncpy(strFileSystem,"ext4",uSize);
            break;
        case SYS_MEDIUM_FS_NTFS:
            strncpy(strFileSystem,"ntfs",uSize);
            break;
        case SYS_MEDIUM_FS_JFS:
            strncpy(strFileSystem,"jfs",uSize);
            break;
        case SYS_MEDIUM_FS_NFS:
            strncpy(strFileSystem,"nfs",uSize);
            break;
        case SYS_MEDIUM_FS_EXFAT:
            /* code */
            strncpy(strFileSystem,"exfat",uSize);
            break;
        default:
            printf("no support type :%d \n",eFsType);
            return -1;
    }
    return 0;
}

/**@fn         sys_medium_mount_exfat  
 * @brief      将EXFAT格式存储设备的分区挂载到目录中
 * @param[in]  strDevPath     设备节点路径  
 * @param[in]  strMountPath   挂载路径  
 * @return     成果返回OK，失败返回ERROR,
 */
INT sys_medium_mount_exfat(const char *strDevPath,const char *strMountPath)
{
    INT iRet = -1;
    char strMountCmd[64] = {0};
    if(!strDevPath  || !strMountPath)
    {
        return iRet;
    }

    snprintf(strMountCmd,sizeof(strMountCmd),"mount.exfat %s %s -o nonempty",strDevPath,strMountPath);
    iRet = sys_posix_cmd_call(strMountCmd);
    if(iRet < 0)
    {
        /* printf("sys_posix_cmd_call:%s error \n",strMountCmd); */
        return iRet;
    }
    return iRet;
}

/**@fn         sys_medium_mount     
 * @brief      将存储设备的分区挂载到目录中，目前只支持一个介质一个分区
 * @param[in]  strDevPath     节点路径
 * @param[in]  eFsType     文件系统类型
 * @param[in]  strMountPath挂载路径
 * @return     成果返回OK，失败返回ERROR,
 */
INT sys_medium_mount(const CHAR *strDevPath, SYS_MEDIUM_FS_TYPE_E eFsType, const CHAR *strMountPath,REPORT_FSINFO_T* fsinfo)
{
    INT iRet = -1;
    CHAR strData[128] = {0};
    CHAR strFileSystem[16] = {0};
     UINT64  start,end;

	if( NULL == strMountPath || NULL == strDevPath )
	{
		return iRet;
	}
   	
    //mount path
    
    iRet = sys_medium_fs_type_convert_string(eFsType,strFileSystem,sizeof(strFileSystem));
    if(iRet < 0)
    {
        printf("sys_medium_fs_type_convert_string type :%d error \n", eFsType);
        return iRet;
    }

    iRet = sys_medium_fs_type_convert_data(eFsType,strData,sizeof(strData));
    if(iRet < 0)
    {
        printf("convert data type :%d error \n", eFsType);
        return iRet;
    }


    if(SYS_MEDIUM_FS_EXFAT == eFsType)
    {   
        sys_time_get_msec(&start);
        iRet = sys_medium_mount_exfat(strDevPath, strMountPath);
        sys_time_get_msec(&end);
        return iRet;
    }

    if(SYS_MEDIUM_FS_NTFS == eFsType)
    {
        sys_time_get_msec(&start);
        iRet = mount(strDevPath, strMountPath, strFileSystem, MS_MGC_VAL | MS_NOATIME, NULL);
         sys_time_get_msec(&end);
    }
    else
    {
         //noatime,async,codepage=936,iocharset=gb2312
        sys_time_get_msec(&start);
        iRet = mount(strDevPath, strMountPath, strFileSystem, MS_MGC_VAL | MS_NOATIME, strData);
        sys_time_get_msec(&end);
    }
    fsinfo->mount_time = (INT)(end - start);
    if(iRet != OK)
    {
        printf("mount %s %s :%s eFsType:%d failed, %s\n", strDevPath, strMountPath,strFileSystem,eFsType, strerror(errno));
        return ERROR;
    }
    return iRet;
}

/**@fn         sys_medium_umount    
 * @brief      卸载被挂载的存储介质，目前只支持一个介质一个分区
 * @param[in]  strMountPath 挂载路径
 * @return     成果返回OK，失败返回ERROR
 */
INT sys_medium_umount(const CHAR *strMountPath)
{
    INT iRet = ERROR;
	if( NULL == strMountPath )
	{
		return iRet;
	}
    if ((iRet = sys_posix_access(strMountPath)) < 0)
    {   
        printf("dev node path :%s \n",strMountPath);
        return iRet;
    }

    iRet = umount(strMountPath);
    if(iRet != OK)
    {
        /* printf("umount %s failed, %s\n", strMountPath, strerror(errno)); */
        return iRet;
    }
    return iRet;
}

/**@fn         sys_medium_make_part    
 * @brief      制作介质分区
 * @param[in]  strDevPath 介质节点  
 * @param[in]  iPartNum  分区个数  
 * @return     成功OK, 其他参考ERROR_CODE_E, 支持get_last_errno获取错误码
 */
INT sys_medium_make_part(const CHAR *strDevPath, INT iPartNum)
{
    INT iRet = ERROR;
	if( NULL == strDevPath )
	{
		return iRet;
	}
	
    iRet = fdisk_make_part(strDevPath, iPartNum);
    if (iRet != OK)
    {
        printf("fdisk_make_part %s failed, %s\n", strDevPath, strerror(errno));
        return ERROR;
    }
   
    return OK;
}

/**@fn         sys_medium_format
 * @brief      格式化介质
 * @param[in]  strDevPath节点路径
 * @param[in]  eFsType   文件系统类型 
 * @param[in]  unit size 分配单元大小
 * @return     成功OK, 其他参考ERROR_CODE_E, 支持get_last_errno获取错误码
 */
INT sys_medium_format(const CHAR *strDevPath, SYS_MEDIUM_FS_TYPE_E eFsType, UINT uUintSize)
{
    INT iRet = -1;
    char strFileSystem[8] ={0};
    char strExtendCmd[32] = {0};
    char strSysFormatCmd[64] = {0};
	UINT uBlockSize = 512;
	UINT uBlockNum = 0;
    uBlockNum =  uUintSize / uBlockSize;

	if( NULL == strDevPath )
	{
		return ERROR;
	}
    switch(eFsType)
    {
        case SYS_MEDIUM_FS_VFAT:
            if(uBlockNum == 0)
            {
                strncpy(strExtendCmd,"-F 32",sizeof(strExtendCmd));    //FAT 32 默认 
            }
            snprintf(strExtendCmd, sizeof(strExtendCmd) - 1 ,"-F 32 -s %d", uBlockNum);
            break;
        case SYS_MEDIUM_FS_EXT:
        case SYS_MEDIUM_FS_EXT2:
        case SYS_MEDIUM_FS_EXT3:
        case SYS_MEDIUM_FS_EXT4:
            strncpy(strExtendCmd,"-m 0.05 -s 256",sizeof(strExtendCmd)); //ext预留分区 百分之5
            break;
        case SYS_MEDIUM_FS_EXFAT:
            if(uBlockNum == 0)
            {
                 strncpy(strExtendCmd,"-s 8192",sizeof(strExtendCmd)); //ext预留分区 百分之5,默认簇大小4MB
            }
            snprintf(strExtendCmd, sizeof(strExtendCmd) - 1 ,"-s %d", uBlockNum);
            break;
        default:
            break;
    }
#ifndef BUSYBOX_VERSION_1_26_2
    iRet = sys_medium_fs_type_convert_string(eFsType,strFileSystem,sizeof(strFileSystem));
    if(iRet < 0)
    {
        printf("sys_medium_fs_type_convert_string type :%d error \n", eFsType);
        return iRet;
    }
#else
    strncpy(strFileSystem,"fat",sizeof(strFileSystem) - 1);
    iRet = OK;
#endif
    
    //fat 32 16  12 格式化为FAT分区,
    //0代表全盘格式 其他数字代表分区格式
    snprintf(strSysFormatCmd,sizeof(strSysFormatCmd),"mkfs.%s %s %s",strFileSystem,strExtendCmd,strDevPath);
    iRet = sys_posix_cmd_call(strSysFormatCmd);
    if(iRet < 0)
    {
        printf("strSysFormatCmd %s error \n", strSysFormatCmd);
        return iRet;
    }
    sync();
    return iRet;
}
/* @brief       设备节点检查  不同平台不一致
 * @param[in]  strDevNodePath 设备节点
 * @return 成果返回OK，失败返回ERROR
 */
INT sys_medium_access_dev_node(const CHAR* strDevNodePath)
{
    INT iRet = ERROR;
    if(!strDevNodePath)
    {
        return iRet;
    }
    
    iRet = open(strDevNodePath,O_RDWR | O_NOCTTY | O_NDELAY);
    if(iRet < 0)
    {
        /* printf("open %s failed, %s\n", strDevNodePath, strerror(errno)); */
        return iRet;
    }
    
    close(iRet);
    return OK;
}
/**@brief      判断设备节点是否存在
 * @param[in]  strDevPath   节点路径  
 * @return     成果返回OK，失败返回ERROR,
 */
INT sys_medium_check_dev_node(const CHAR *strDevPath)
{
    /*规避措施 后续还是要去掉*/
	INT iRet = ERROR;
    INT iCnt = 0;

	if( NULL == strDevPath )
	{
		return ERROR;
	}
    /*规避措施 后续还是要去掉*/
	for(iCnt = 0; iCnt < 2; iCnt++)        /* 当前尝试两次 */
	{
		if(sys_medium_access_dev_node(strDevPath) == OK)
		{
			printf("check dev node:%s successful\n", strDevPath);
			return 0;
		}

		/* SYS_COMMON_INFO("check dev node:%s failed and try again, times = %d\n", strDevPath, iCnt); */
		//sys_time_sleep_ms(100);
	}
    return iRet;
}

/**@fn         sys_medium_get_info
 * @brief      medium获取介质信息
 * @param[in]  strPath       节点路径
 * @param[out] pStMediumInfo 介质信息指针  
 * @return     成功 ok 失败 ERROR
 */
INT sys_medium_get_info(const CHAR *strMountPath, SYS_MEDIUM_INFO_T *pStMediumInfo,REPORT_FSINFO_T* fsinfo)
{
    INT iRet = -1;
    UINT uDiskTotalSize = 0;
    UINT uDiskFreeSize = 0;
    struct statfs stInfo = {0};
    UINT64  start,end;
    if(!pStMediumInfo || !strMountPath)
    {
        return iRet;
    }
    sys_time_get_msec(&start);
    iRet = statfs(strMountPath, &stInfo);
    sys_time_get_msec(&end);
    if (iRet< 0)
    {
        pStMediumInfo->uTotalSize = 0;
        pStMediumInfo->uRemainSize = 0;
        pStMediumInfo->eType = SYS_MEDIUM_FS_TYPE_UNKNOWN;
        return iRet;
    }
    fsinfo->statfs_time = (int)(end-start);

    if(stInfo.f_blocks < 1)
    {
        pStMediumInfo->uTotalSize = 0;
        pStMediumInfo->uRemainSize = 0;
        pStMediumInfo->eType = SYS_MEDIUM_FS_TYPE_UNKNOWN;
        return iRet;
    }
    
    /* 计算总容量时屏蔽根目录所占的一个簇的大小 */
    if(stInfo.f_bsize >= 1024)
    {
       uDiskTotalSize = (UINT)((stInfo.f_bsize / 1024 ) * (stInfo.f_blocks - 1));
       uDiskFreeSize = (UINT)(( stInfo.f_bsize / 1024 ) * stInfo.f_bavail);
    }
    else
    {
        
       uDiskTotalSize = (UINT)(((stInfo.f_blocks - 1) / 1024) * stInfo.f_bsize);
       uDiskFreeSize = (UINT) ((stInfo.f_bavail / 1024) * stInfo.f_bsize);
    }
    
   // SYS_COMMON_INFO("stInfo.f_bsize = %u\n", stInfo.f_bsize ); 
    pStMediumInfo->uClusterSize = stInfo.f_bsize;
    pStMediumInfo->uTotalSize = uDiskTotalSize/1024;
    pStMediumInfo->uRemainSize = uDiskFreeSize/1024;
    switch (stInfo.f_type)
    {
        case SYS_MEDIUM_FS_MAGIC_EXFAT:
            pStMediumInfo->eType = SYS_MEDIUM_FS_EXFAT;
            break;
        case SYS_MEDIUM_FS_MAGIC_FAT32:
            pStMediumInfo->eType = SYS_MEDIUM_FS_VFAT;
            break;
        case SYS_MEDIUM_FS_MAGIC_EXT4:
            pStMediumInfo->eType = SYS_MEDIUM_FS_EXT4;
            break;
        default:
            break;
    }
    
    printf("strMountPath:%s uTotalSize:%dMB uRemainSize:%dMB f_type:0x%x ClusterSize:%d\n",strMountPath,pStMediumInfo->uTotalSize,pStMediumInfo->uRemainSize,(UINT)stInfo.f_type,pStMediumInfo->uClusterSize);
    return 0;
}

/**@fn         sys_medium_sync
 * @brief      强制写文件到存储介质      
 * @param[in]  strPath  盘符路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_medium_sync(const CHAR *strPath)
{
    INT iRet = ERROR;
    if(NULL == strPath)
    {
        return iRet;
    } 

    sync();
    return OK;
}