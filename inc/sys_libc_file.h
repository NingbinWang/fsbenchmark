#ifndef _SYS_LIBC_FILE_H_
#define _SYS_LIBC_FILE_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include "sys_time.h"
#include "utils.h"
#include "report.h"

#ifndef FILE_WINDOWS
#define FILE_SEARCH_SEPARATOR ('/')
#else
#define FILE_SEARCH_SEPARATOR ('\\')
#endif

/**@struct SYS_FS_ATTR_E 
 * @brief  定义文件属性枚举
 */
typedef enum {
    /* Standard file attribute */
    SYS_FS_ATTR_RDONLY         =   0x01,
    SYS_FS_ATTR_HIDDEN         =   0x02,
    SYS_FS_ATTR_SYSTEM         =   0x04,
    SYS_FS_ATTR_VOLUME         =   0x08,
    SYS_FS_ATTR_DIR            =   0x10,
    SYS_FS_ATTR_ARCH           =   0x20,

    /* Other file attribute */
    SYS_FS_ATTR_NONE           =   0x40,
    SYS_FS_ATTR_ALL            =   0x7f,
    SYS_FS_ATTR_FILE_ONLY      =   0x100,
} SYS_FS_ATTR_E;

/**@struct SYS_FS_STAT_T 
 * @brief  定义文件属性结构
 */
typedef struct
{
    UINT64 uSize;               /*文件实际大小,单位Byte*/
    UINT uAttr;               /*位图，参考SYS_FS_ATTR_T*/
    UINT uClusterSize;        /*文件簇大小,单位Byte*/
    UINT64 uSpaceSize;          /*文件实际占用空间大小,单位 Byte,大小为文件大小的整数倍*/
    UINT uRes[4];             /*预留字节*/
}SYS_FS_STAT_T;

typedef LONG  FILE_ID;
typedef LONG  DIR_ID;

/*文件位置偏移current*/
#ifndef SEEK_CUR
#define SEEK_CUR             (1)             
#endif

/*文件位置偏移到头部*/
#ifndef SEEK_SET
#define SEEK_SET             (0)
#endif

/*文件位置偏移到尾部*/
#ifndef SEEK_END
#define SEEK_END             (2)
#endif

/**@fn         ReadDirCallBack      
 * @brief      读目录文件
 * @param[in]  strDir          查看目录
 * @param[in]  pUserParam      自定义参数
 * @return     成功 0  失败 -1 
 */
typedef INT (*ReadDirCallBack)(const CHAR *strDirPath, VOID *pUserParam);

/**@fn         ReadFileCallBack      
 * @brief      遍历目录文件信息
 * @param[in]  strFilePath     文件全路径
 * @param[in]  pStFileState    文件状态信息指针
 * @param[in]  pUserParam      自定义数据
 * @return       成功 0  失败 -1 
 */
typedef INT (*ReadFileCallBack)(const CHAR *strFilePath, SYS_FS_STAT_T *pStFileState, VOID *pUserParam);




FILE_ID* sys_libc_fopen(const CHAR *strPath, const CHAR *strMode);
VOID fopen_bench(REPORT_LIBCAPI_T* info);

INT sys_libc_fclose(FILE_ID *pFileID);
VOID fclose_bench(REPORT_LIBCAPI_T* info);

INT sys_libc_fwrite(FILE_ID *pFileID, const VOID *pBuffer, UINT uSize, UINT uCount);
VOID fwrite_bench(REPORT_LIBCAPI_T* info);

INT sys_libc_fread(FILE_ID *pFileID, VOID *pBuffer, UINT uSize, UINT uCount);
VOID fread_bench(REPORT_LIBCAPI_T* info);

INT sys_file_fsync(FILE_ID *pFileID);
VOID fsync_bench(REPORT_SYNC_T* info);


INT sys_file_fstat(const CHAR *strPath, SYS_FS_STAT_T *pStSate);
VOID fstat_bench(REPORT_LIBCAPI_T* info);


INT sys_file_read_dir_file(const CHAR *strDir, VOID *pUserParam, ReadFileCallBack pCallBack,REPORT_FSINFO_T *fsinfo);



#endif