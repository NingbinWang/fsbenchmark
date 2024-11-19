/**@file    sys_pthread.h
 * @brief   posix接口中间层宏定义接口
 * @method  1.创建线程
 *          2.创建非线程分离线程，线程结束后不再释放自己的资源
 *          3.设置线程取消类型
 *          4.设置线程取消状态
 *          5.等待线程结束
 *          6.取消线程并删除相关资源
 *          7.验证线程是否存在
 *          8.获取调用线程的ID
 *          9.恢复阻塞的线程
 *          10.阻塞线程
 * @author  wch
 * @version V1.0.0 wch 2020-2-2   根据编码规范整理接口
 */
#ifndef _SYS_PTHREAD_H_
#define _SYS_PTHREAD_H_
#include "sys_common.h"

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* TASK_PRIORITY_0最高优先级 , 下面只是通用定义，各平台独立进行映射*/
typedef enum  {
    TASK_PRIORITY_0 = 0,
    TASK_PRIORITY_1 = 1,
    TASK_PRIORITY_2 = 2,
    TASK_PRIORITY_3 = 3,
    TASK_PRIORITY_4 = 4,
    TASK_PRIORITY_5 = 5,
    TASK_PRIORITY_6 = 6,
    TASK_PRIORITY_7 = 7,
    TASK_PRIORITY_8 = 8,
    TASK_PRIORITY_9 = 9,
    TASK_PRIORITY_NUM,
}TASK_PRIORITY_E;

/* TASK_ID 仅仅一个指针值，具体指向什么各平台内部处理*/
typedef LONG  TASK_ID;

typedef VOID (*FUNCPTR)(VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*, VOID*);

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
        unsigned args, ...);

/**@fn         sys_pthread_join
 * @brief      等待线程结束
 * @param[in]  stTaskId   任务ID句柄
 * @return     成功返回 0  错误返回 其他
 */
INT sys_pthread_join(TASK_ID stTaskId);

/**@fn         sys_pthread_cancel
 * @brief      取消线程并删除相关资源
 * @param[in]  stTaskId   任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_pthread_cancel(TASK_ID stTaskId);

/**@fn         sys_pthread_verify
 * @brief      验证线程是否存在
 * @param[in]  pStTid     任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_pthread_verify(TASK_ID *pStTid);

/**@fn         sys_pthread_suspend
 * @brief      阻塞线程
 * @param[in]  pStTid        任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_pthread_suspend(TASK_ID *pStTid);


/**@fn         sys_pthread_self
 * @brief      获取调用线程的ID
 * @return     成功返回 0  返回线程对象ID
 */
TASK_ID sys_pthread_self(VOID);

/**@fn         sys_pthread_reume
 * @brief      恢复阻塞的线程
 * @param[in]  pStTid      任务ID句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_pthread_reume(TASK_ID *pStTid);

/**@fn         sys_pthread_get_name
 * @brief      获取当前调用线程线程名信息
 * @param[out] strName     线程任务名称
 * @param[in]  uNameLen    线程名称还清楚大小
 * @return     成功返回0,错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_pthread_get_name( CHAR *strName, UINT16 uNameLen);

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/*_SYS_POSIX_H_*/