#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pwd.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fs.h>
#include <signal.h>
#include <getopt.h>
#include "sys_libc_file.h"
#include "sys_medium.h"
#include "sys_pthread.h"
#include  "report.h"

#define FSBENCH_VERSION     "1.0.1"
#define MOUNT_PATH          "/tmp/mmc03"
#define RANDOM_PATH         "/dev/urandom"

typedef struct
{   
    UINT                   index;
    UINT                   prejobsize;
    UINT                   prejobfilenum;
}THEAD_PARAM_T;

int flag[64] = {0};

void Testlibc_func(THEAD_PARAM_T* param)
{
    int i = 0;
    int j = 0;
    int count ;
    FILE_ID* handle[64];
    char filepath[128];
     count = MAXPREFILESIZE*1024/PERWR_SIZE;
    for(i = 0;i < param->prejobfilenum;i++)
    {
        int fd;
        FILE_ID* handle;
        char buff[PERWR_SIZE*SIZE_1KB];
        fd = open(RANDOM_PATH,O_RDONLY);
        read(fd,buff,sizeof(buff));
        close(fd);
        snprintf(filepath, sizeof(filepath) - 1 ,"%s/test%d.mp4",MOUNT_PATH,param->index*64+i);
        handle =  sys_libc_fopen(filepath, "w+");
         printf("start  TestlibcAPI_func%d: %d/%d please wait ........... \n",param->index,i,param->prejobfilenum-1);
        for(j = 0;j < count;j++)
        {
            sys_libc_fwrite(handle,buff,PERWR_SIZE*SIZE_1KB,1);
            sys_file_fsync(handle);
            sys_posix_sync();
            sys_libc_fread(handle,buff,PERWR_SIZE*SIZE_1KB,1);
           // printf("Testlibc_func%d:rw: %d/%d \n",param->index,j,count);
        }
       
        sys_libc_fclose(handle);
    }
    printf("TestlibcAPI_func%d: test OK\n",param->index);
    flag[param->index] = 1;
}

void rename_test(char* filename,SYS_FS_STAT_T stat,VOID *pUserParam)
{
   printf("filename:%s\n",filename);
}


static void show_version(void)
{
    printf("fsbench %s\n", FSBENCH_VERSION);
}

static int usage(void)
{
    fprintf(stderr, 
      "\nUsage: fsbench [option]\n\n"
      "    -v | --version       show version\n"
      "    -h | --help          Print this message\n"
      "    -d | --devname [arg] devname; like /dev/mmblk0p1\n"
      "    -j | --jobs           read/write pthread 1~64\n"
      "    -p | --bypass           don't test open read write sync api \n"
      "\n"
      "\n"
      "example:\n"
      "  fsbench -d /dev/mmblk0p1 -j 1\n"
    );
}


int main(int argc, char *const argv[])
{
    int c                   = 0;
    int idx                 = 0;
    char* devname           = NULL;
    int jobs_num            = 0;
    int nobypass              = 1;
    int ret                 = -1;
    SYS_MEDIUM_INFO_T      fsinfo;
    BENCH_REPORT_T         report;
    UINT                   i;
    TASK_ID                taskid[64];
    THEAD_PARAM_T   threadparam[64];
    UINT                   prejobsize;
    UINT                   prejobfilenum;
   
    const char short_options[] = "hvpd:j:s:m:l:";
    const struct option long_options[] = {
        { "help",       no_argument,        NULL, 'h' },
        { "version",    no_argument,        NULL, 'v' },
        { "pass",       no_argument,        NULL, 'p' },
        { "devname",    required_argument,  NULL, 'd' },
        { "jobs",       required_argument,  NULL, 'j' },
        { 0, 0, 0, 0 },
    };
    if (argc == 1)
    {
        printf("args num is 1.\n");
        return usage();
    }

#if defined(__GLIBC__) || defined(__linux__)
    optind = 0;
#else
    optind = 1;
#endif
    if (argc > 1)
    {
        while ((c = getopt_long(argc, argv, short_options, long_options, &idx)) != EOF)
        {
            switch(c)
            {
                case 'd':
                    devname = optarg;
                    break;
                case 'v':
                    show_version();
                    return 0;
                case 'j':
                    jobs_num  = strtoull(optarg, NULL, 0);
                    if (jobs_num < 1 || jobs_num > 64)
                    {
                        printf("args jobserror %d.\n", jobs_num);
                        return usage();
                    }
                    printf("job = %d\n",jobs_num);
                    
                    break;
                case 'p':
                    nobypass = 0;
                    break;
                case 'h':
                default:
                    printf("c[0x%x]\n", c);
                    usage();
                    return -1;
            }
        }
    }

    if (devname == NULL)
    {
        printf("args devname[%p]  is error.\n", devname);
        return usage();
    }
// step1 mount dev
    ret = sys_medium_check_dev_node(devname);
    if(ret)
    {
       printf("no find dev node\n");
    } 
    sys_medium_umount(MOUNT_PATH);
    sys_posix_mkdir(MOUNT_PATH);
    ret = sys_medium_mount(devname,SYS_MEDIUM_FS_VFAT,MOUNT_PATH,&(report.fsinfo_report));
    if(ret)
    {
        printf("mount error\n");
        //goto mounterror;
    }
    

//step2 get media info
    ret = sys_medium_get_info(MOUNT_PATH,&fsinfo,&(report.fsinfo_report));
   if(ret)
    {
        printf("get info error\n");
        goto getfsinfoerror;
    }
    //if it have only 1/10 total size , must mkfs it.
//step3  alloction
   printf("jobs = %d\n",jobs_num);;
   prejobsize = fsinfo.uRemainSize/jobs_num;
   printf("prejob size = %dMB\n",prejobsize);
   prejobfilenum = prejobsize/MAXPREFILESIZE;
   printf("prejob filenum = %d\n",prejobfilenum);
//step4 create pthread to create file and write file then read file 
   if(nobypass)
   {
      for(i = 0 ;i < jobs_num;i++)
      {
            char taskname[128];
            threadparam[i].index = i;
            threadparam[i].prejobsize = prejobsize;
            threadparam[i].prejobfilenum = prejobfilenum;
            snprintf(taskname, sizeof(taskname) - 1 ,"fsbenchtest%d", i);
            printf("taskname=%s,index:%d\n",taskname,threadparam[i].index);
            sys_pthread_create(&taskid[i],taskname,TASK_PRIORITY_1,SIZE_1MB,Testlibc_func,1,&threadparam[i]);
      }
      for(i = 0 ;i < jobs_num;i++)
      {
         sys_pthread_join(taskid[i]);
      }
 
      int count = 0;
      while(1)
      {
        if(flag[count++] == 0)
         {
            count = 0;
            continue;
        }
        if(count == (jobs_num-1))
        break;
      }
   }
//step5 readdir every file
sys_file_read_dir_file("/tmp/mmc03/",(void *)devname,rename_test,&(report.fsinfo_report));
//step6 report
printf("fsinfo cost mount %dms, statfs %dms, opendir: %dms readdir:%dms\n",report.fsinfo_report.mount_time,report.fsinfo_report.statfs_time,report.fsinfo_report.opendir_time,report.fsinfo_report.readdir_time);

if(nobypass)
{
  fopen_bench(&(report.fopen_report));
  fclose_bench(&(report.fclose_report));
  fwrite_bench(&(report.fwrite_report));
  fread_bench(&(report.fread_report));
  fsync_bench(&(report.fsync_report));
  sync_bench(&(report.sync_report));
  printf("fopen cost min %dms, max %dms, avg: %dms\n",report.fopen_report.min,report.fopen_report.max,report.fopen_report.avg);
  printf("fwrite cost min %dms, max %dms, avg: %dms\n",report.fwrite_report.min,report.fwrite_report.max,report.fwrite_report.avg);
  printf("fread cost min %dms, max %dms, avg: %dms\n",report.fread_report.min,report.fread_report.max,report.fread_report.avg);
  printf("fsync cost fflush min %dms, max %dms, avg: %dms fileno min %dms, max %dms, avg: %dms fsync min %dms, max %dms, avg: %dms\n", \
      report.fsync_report.fflush_min,report.fsync_report.fflush_max,report.fsync_report.fflush_avg, \
      report.fsync_report.fileno_min,report.fsync_report.fileno_max,report.fsync_report.fileno_avg, \
      report.fsync_report.fsync_min,report.fsync_report.fsync_max,report.fsync_report.fsync_avg);
  printf("sync_bench cost min %dms, max %dms, avg: %dms\n",report.sync_report.min,report.sync_report.max,report.sync_report.avg);
  printf("fclose_bench cost min %dms, max %dms, avg: %dms\n",report.fclose_report.min,report.fclose_report.max,report.fclose_report.avg);
}
fstat_bench(&(report.fstat_report));
printf("fstat cost min %dms, max %dms, avg: %dms\n",report.fstat_report.min,report.fstat_report.max,report.fstat_report.avg);




getfsinfoerror:
    sys_medium_umount(MOUNT_PATH);
mounterror:
   return 0;
}