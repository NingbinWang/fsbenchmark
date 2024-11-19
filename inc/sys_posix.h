/**@file    sys_posix.h
 * @brief   posix接口中间层宏定义接口
 * @method  1.verity判断是否进程存在
 *          2.kill杀死进程
 *          3.ioctl封装
 *          4.system系统命令调用
 *          5.move移动文件
 *          6.mkdir创建目录
 *          7.rmdir删除目录
 *          8.fexist判断文件是否存在
 *          9.rm删除
 *          10.rename重命名
 *          11.sync介质数据同步
 */
#ifndef _SYS_POSIX_H_
#define _SYS_POSIX_H_
#include "sys_common.h"
#include "report.h"

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

#define SYS_FD_SETSIZE    1024

typedef struct 
{
    UINT8 fd_bits [(SYS_FD_SETSIZE+7)/8];
}sys_fd_set;

struct sys_timeval {
  LONG tv_sec;       /* seconds */
  LONG tv_usec;      /* microseconds */
};

/**@fn         sys_posix_kill
 * @brief      调用系统命令,如果命令的前缀为$，则进程在后台执行
 * @param[in]  iPid 进程ID
 * @return     无
 */
VOID sys_posix_kill(INT iPid);

/**@fn         sys_posix_verify
 * @brief      判断进程是否存在
 * @param[in]  iPid 进程ID
 * @return     存在返回OK，不存在返回ERROR
 */
INT sys_posix_verify(INT iPid);

/**@fn         sys_posix_ioctl
 * @brief      底层数据交互接口,IO控制接口
 * @param[in]  iFd      设备句柄
 * @param[in]  uCmd     命令
 * @param[out] pData    数据缓存
 * @return     存在返回OK，不存在返回ERROR
 */
INT sys_posix_ioctl(INT iFd, UINT uCmd,VOID *pData);

/**@fn         sys_posix_cmd_call
 * @brief      调用系统命令,如果命令的前缀为$，则进程在后台执行
 * @param[in]  strCmd 系统命令字符串
 * @return     调用失败，返回-1；调用成功，若后台执行返回大于0的子进程号，前台执行则返回system接口返回值
 */
INT sys_posix_cmd_call(const CHAR *strCmd);

/**@fn         sys_posix_cp
 * @brief      拷贝文件 
 * @param[in]  strSrcPath  待移动文件全路径
 * @param[in]  strDstPath  目的路径,全路径
 * @return     成功返回 0   错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_posix_cp(const CHAR *strSrcPath, const CHAR *strDstPath);

/**@fn         sys_posix_move
 * @brief      移动文件 
 * @param[in]  strSrcPath  待移动文件全路径
 * @param[in]  strDstPath  目的路径,全路径
 * @return     成功返回 0   错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_posix_move(const CHAR *strSrcPath, const CHAR *strDstPath);

/**@fn         sys_posix_mkdir
 * @brief      创建目录
 * @param[in]  strPath 路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_posix_mkdir(const CHAR *strPath);

/**@fn         sys_posix_rmdir
 * @brief      删除目录
 * @param[in]  strPath 路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT sys_posix_rmdir(const CHAR *strPath);

/**@fn         sys_posix_rm
 * @brief      删除
 * @param[in]  strPath 文件路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_ID_E
 */
INT sys_posix_rm(const CHAR *strPath);





VOID sync_bench(REPORT_LIBCAPI_T* info);
/**@fn          sys_posix_sync      
 * @brief       强制写所有文件到存储介质
 * @return      成功 0  失败 -1 
 */
VOID sys_posix_sync(VOID);

/**@fn         sys_posix_rename
 * @brief      重命名文件
 * @param[in]  strOldPath 旧文件路径
 * @param[in]  strNewPath 新文件路径
 * @return     成功返回 0  错误返回 其他, 参考ERROR_ID_E
 */
INT sys_posix_rename(const CHAR *strOldPath, const CHAR *strNewPath);

/**@fn         sys_posix_access
 * @brief      判断文件是否存在
 * @param[in]  strPath 路径
 * @return     存在返回 0 失败返回 其他
 */    
INT sys_posix_access(const CHAR *strPath);

/**@fn         sys_posix_fcntl
 * @brief      设置套接字属性，参考man fcntl
 * @param[in]  iFd      套接字句柄
 * @param[in]  iType    操作层面，比如是套接字层面(SOL_SOCKET)还是协议层面
 * @param[in]  iOptions 操作名称
 * @return     成功返回对应的值；失败参考ERROR_ID_E
 */
INT sys_posix_fcntl(INT iFd, UINT iType, UINT iOptions);

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
                  sys_fd_set *pStExceptionFdSet, struct sys_timeval *pStTimeOut);

/**@fn         sys_posix_get_last_errno      
 * @brief      获取错误码
 * @param[in]  无
 * @return     详见errno 
 */
INT sys_posix_get_last_errno();

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/*_SYS_POSIX_H_*/