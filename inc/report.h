#ifndef _REPORT_H_
#define _REPORT_H_

#include "sys_common.h"

#define MAX_FOPEN    64*1024
#define MAX_FCLOSE   64*1024

#define MAX_FREAD   64*1024
#define MAX_FWRITE  64*1024
#define MAX_FSYNC   64*1024
#define MAX_FSTAT   64*1024
#define MAX_SYNC    64*1024

#define MAXPREFILESIZE      270 //SIZE:1MB every file max size
#define PERWR_SIZE          256 //SIZE:1KB every time use write or read




typedef struct
{
    INT avg;
    INT min;
    INT max;
    long count;
}REPORT_LIBCAPI_T;

typedef struct
{
    INT fflush_avg;
    INT fflush_max;
    INT fflush_min;
    INT fileno_avg;
    INT fileno_max;
    INT fileno_min;
    INT fsync_avg;
    INT fsync_max;
    INT fsync_min;
    long count;
}REPORT_SYNC_T;


typedef struct
{
    INT mount_time;
    INT statfs_time;
    INT opendir_time;
    INT readdir_time;
}REPORT_FSINFO_T;



typedef struct
{   
    REPORT_LIBCAPI_T          fopen_report;
    REPORT_LIBCAPI_T          fclose_report;
    REPORT_LIBCAPI_T          fread_report;
    REPORT_LIBCAPI_T          fwrite_report;
    REPORT_SYNC_T             fsync_report;
    REPORT_LIBCAPI_T          sync_report;
    REPORT_LIBCAPI_T          fstat_report;
    REPORT_FSINFO_T           fsinfo_report;
}BENCH_REPORT_T;

#endif