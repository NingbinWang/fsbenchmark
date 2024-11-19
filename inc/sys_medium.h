/**@file     sys_medium.h
 * @brief   定义介质操作相关接口
 *          1.支持磁盘分区
 *          2.支持磁盘格式化
 *          3.支持磁盘校验
 *          4.支持磁盘挂载、卸载
 *          5.支持获取磁盘信息
 *          6.支持挂在分区
 *          7.支持卸载分区
 */

#ifndef _SYS_MEDIUM_H_
#define _SYS_MEDIUM_H_
#include"sys_common.h"
#include "report.h"

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/**@enum  SYS_MEDIUM_FS_TYPE_E 
 * @brief 文件系统类型枚举值定义，支持ext, ext2, ext3, ext4, hpfs, iso9660, JFS, minix, msdos, ncpfs nfs, ntfs, proc, Reiserfs, smb, sysv, umsdos, vfat, XFS, xiafs
 */
typedef enum
{
    SYS_MEDIUM_FS_EXT = 0,
    SYS_MEDIUM_FS_EXT2,
    SYS_MEDIUM_FS_EXT3,
    SYS_MEDIUM_FS_EXT4,
    SYS_MEDIUM_FS_HPFS,
    SYS_MEDIUM_FS_IOS9660,
    SYS_MEDIUM_FS_JFS,
    SYS_MEDIUM_FS_MINIX,
    SYS_MEDIUM_FS_MSDOS,
    SYS_MEDIUM_FS_NCPFS,
    SYS_MEDIUM_FS_NFS,
    SYS_MEDIUM_FS_NTFS,
    SYS_MEDIUM_FS_PROC,
    SYS_MEDIUM_FS_REISERFS,
    SYS_MEDIUM_FS_SMB,
    SYS_MEDIUM_FS_SYSV,
    SYS_MEDIUM_FS_UMSDOS,
    SYS_MEDIUM_FS_VFAT,
    SYS_MEDIUM_FS_XFS,
    SYS_MEDIUM_FS_XIAFS,
    SYS_MEDIUM_FS_EXFAT,
    SYS_MEDIUM_FS_TYPE_UNKNOWN
}SYS_MEDIUM_FS_TYPE_E;

/**@struct SYS_MEDIUM_INFO_T 
 * @brief  介质信息结构体定义，主要包括文件系统类型、总大小、剩余大小、簇大小
 */
typedef struct
{   
    SYS_MEDIUM_FS_TYPE_E eType;/*文件系统类型*/
    UINT uTotalSize;   /*单位MB*/
    UINT uRemainSize;  /*单位MB*/
    UINT uClusterSize; /*簇大小，单位B*/
}SYS_MEDIUM_INFO_T;

/**@fn         sys_medium_mount     
 * @brief      将存储设备的分区挂载到目录中，目前只支持一个介质一个分区
 * @param[in]  strPath     节点路径
 * @param[in]  eFsType     文件系统类型
 * @param[in]  strMountPath挂载路径
 * @return     成果返回OK，失败返回ERROR,
 */
INT sys_medium_mount(const CHAR *strPath, SYS_MEDIUM_FS_TYPE_E eFsType, const CHAR *strMountPath,REPORT_FSINFO_T* fsinfo);

/**@fn         sys_medium_umount    
 * @brief      卸载被挂载的存储介质，目前只支持一个介质一个分区
 * @param[in]  strMountPath挂载路径
 * @return     成果返回OK，失败返回ERROR
 */
INT sys_medium_umount(const CHAR *strMountPath);

/**@fn         sys_medium_make_part    
 * @brief      制作介质分区
 * @param[in]  strDevPath 介质节点  
 * @param[in]  iPartNum  分区个数  
 * @return     成功OK, 其他参考ERROR_CODE_E, 支持get_last_errno获取错误码
 */
INT sys_medium_make_part(const CHAR *strDevPath, INT iPartNum);

/**@fn         sys_medium_format
 * @brief      格式化介质
 * @param[in]  strPath   节点路径
 * @param[in]  eFsType   文件系统类型 
 * @param[in]  unit size 分配单元大小
 * @return     成功OK, 其他参考ERROR_CODE_E, 支持get_last_errno获取错误码
 */
INT sys_medium_format(const CHAR *strPath, SYS_MEDIUM_FS_TYPE_E eFsType, UINT uUintSize);

/**@brief      判断设备节点是否存在
 * @param[in]  strPath   节点路径  
 * @return     成果返回OK，失败返回ERROR,
 */
INT sys_medium_check_dev_node(const CHAR *strPath);

/**@fn         sys_medium_get_info
 * @brief      medium获取介质信息
 * @param[in]  strPath   节点路径
 * @param[out] pMediumInfo 介质信息指针  
 * @return     成功 ok 失败 ERROR
 */
INT sys_medium_get_info(const CHAR *strMountPath, SYS_MEDIUM_INFO_T *pStMediumInfo,REPORT_FSINFO_T* fsinfo);

/**@fn         sys_medium_sync
 * @brief      强制写文件到存储介质      
 * @param[in]  strPath  盘符路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_medium_sync(const CHAR *strPath);

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/*__SYS_MEDIUM_INTERFACE_H__*/