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

#define FSBENCH_VERSION     "1.0.6"
#define MOUNT_PATH          "/tmp/mmc03"
#define RANDOM_PATH         "/dev/urandom"


typedef struct
{   
    UINT                   index;
    UINT                   prejobsize;
    UINT                   prejobfilenum;
    UINT                   prejobrwsize;
}THEAD_PARAM_T;

int flag[64] = {0};

void Testlibc_func(THEAD_PARAM_T* param)
{
    int i = 0;
    int j = 0;
    int count ;
    char filepath[128];
    FILE_ID* handle[param->prejobfilenum];
    count = (param->prejobsize/param->prejobfilenum)*1024/(param->prejobrwsize);
    for(i = 0;i < param->prejobfilenum;i++)
    {
        snprintf(filepath, sizeof(filepath) - 1 ,"%s/test%d.mp4",MOUNT_PATH,param->index*64+i);
        handle[i] =  sys_libc_fopen(filepath, "w+");
    }
    printf("start  TestlibcAPI_func%d: file number:%d write times:%d prejob write size %d kB please wait ........... \n",param->index,param->prejobfilenum,count,(param->prejobrwsize));
    for(j = 0;j < count;j++)
    {
        int fd;
        char buff[(param->prejobrwsize)*SIZE_1KB];
        fd = open(RANDOM_PATH,O_RDONLY);
        read(fd,buff,sizeof(buff));
        close(fd);
        for(i = 0;i < param->prejobfilenum;i++){
            sys_libc_fwrite(handle[i],buff,(param->prejobrwsize)*SIZE_1KB,1);
            sys_file_fsync(handle[i]);
            sys_posix_sync();
            sys_libc_fread(handle[i],buff,(param->prejobrwsize)*SIZE_1KB,1);
        }
         printf("Testlibc_func%d:rwtest: %d/%d \n",param->index,j,count-1);
    }
    for(i = 0;i < param->prejobfilenum;i++){
       sys_libc_fclose(handle[i]);
    }
    printf("TestlibcAPI_func%d: test OK\n",param->index);
    flag[param->index] = 1;
}

void rename_test(char* filename,SYS_FS_STAT_T stat,VOID *pUserParam)
{
    if(strstr(filename,"test")!= NULL){
       char newfilename[256];
       sprintf(newfilename,"%s_rename",filename);
       sys_posix_rename(filename,newfilename);
    }else{
        printf("resver file: %s\n",filename);
    }
}

void unlink_test(char* filename,SYS_FS_STAT_T stat,VOID *pUserParam)
{
    if(strstr(filename,"rename")!= NULL)
    {
        sys_posix_rm(filename);
    }
}

void fileshow_test(char* filename,SYS_FS_STAT_T stat,VOID *pUserParam)
{
    if(strstr(filename,"comettest")!= NULL)
       sys_posix_rm(filename);
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
      "    -c | --comet [arg] filenumber; like 9000  create 9000 files and readdir\n"
      "    -l | --lenght [arg] perfilesize(/MB); like 270  perfile size\n"
      "    -s | --prerwsize [arg] perrwsize(/KB); like 512  perwrtest size\n"
      "    -f | --outfile [arg] csvfilename; like fsbench.csv \n"
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
    BENCH_REPORT_T         report = {0};
    UINT                   i;
    char*  csvfilename     = NULL;
    TASK_ID                taskid[64];
    THEAD_PARAM_T          threadparam[64];
    UINT                   prejobsize;
    UINT                   prejobfilenum;
    int                    cometfilenum = 0;
    UINT                   filesize =  210;
    UINT                   rwsize =  512;
    
    const char short_options[] = "hvpd:j:c:l:s:f:";
    const struct option long_options[] = {
        { "help",       no_argument,        NULL, 'h' },
        { "version",    no_argument,        NULL, 'v' },
        { "pass",       no_argument,        NULL, 'p' },
        { "devname",    required_argument,  NULL, 'd' },
        { "jobs",       required_argument,  NULL, 'j' },
        { "comet",      required_argument,  NULL, 'c' },
        { "lenght",     required_argument,  NULL, 'l' },
        { "prerwsize",  required_argument,  NULL, 's' },
        { "outfile",    required_argument,  NULL, 'f' },
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
                    break;
                case 'c':
                    cometfilenum  = strtoull(optarg, NULL, 0);
                    if (cometfilenum < 1 || cometfilenum > MAX_FILENUM)
                    {
                        printf("args cometfilenumerror %d.\n", cometfilenum);
                        return usage();
                    }  
                    break;
                case 'p':
                    nobypass = 0;
                    break;
                case 'l':
                    filesize  = strtoull(optarg, NULL, 0);
                    if (filesize < 1 || filesize > 1024)
                    {
                        printf("args filesize error %d.\n", filesize);
                        return usage();
                    }  
                    break;
                case 's':
                    rwsize  = strtoull(optarg, NULL, 0);
                    if (rwsize < 1 || rwsize > 5120)
                    {
                        printf("args filesize error %d.\n", filesize);
                        return usage();
                    }  
                    break;
                case 'f':
                    csvfilename = optarg;
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
        goto mounterror;
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
    if(jobs_num > 0){ 
       printf("thread number = %d prefilesize = %dMB rwtest size=%dKB\n",jobs_num,filesize,rwsize);
       prejobsize = fsinfo.uRemainSize/jobs_num;
       printf("prejob size = %dMB\n",prejobsize);
       prejobfilenum = prejobsize/filesize;
       printf("prejob filenum = %d\n",prejobfilenum);
    }
//step4 create pthread to create file and write file then read file 
   if(nobypass && (jobs_num > 0))
   {
      for(i = 0 ;i < jobs_num;i++)
      {
            char taskname[128];
            threadparam[i].index = i;
            threadparam[i].prejobsize = prejobsize;
            threadparam[i].prejobfilenum = prejobfilenum;
            threadparam[i].prejobrwsize = rwsize;
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
      //step5 readdir every file
      printf("start rename test............\n");
      sys_file_read_dir_file(MOUNT_PATH,(void *)devname,rename_test,&(report.fsinfo_report));
      if (devname != NULL){
          sys_libc_gencsv(csvfilename,"w+");//初始数据
          sys_posix_gencsv(csvfilename,"a+");//posix初始数据
      }
      rename_bench(&(report.rename_report));
      printf("rename_bench cost min %dms, max %dms, avg: %dms\n",report.rename_report.min,report.rename_report.max,report.rename_report.avg);
      printf("fsinfo cost mount %dms, statfs %dms, opendir: %dms readdir:%dms\n",report.fsinfo_report.mount_time,report.fsinfo_report.statfs_time,report.fsinfo_report.opendir_time,report.fsinfo_report.readdir_time);
      fstat_bench(&(report.fstat_report));
      printf("fstat cost min %dms, max %dms, avg: %dms\n",report.fstat_report.min,report.fstat_report.max,report.fstat_report.avg);
      fopen_bench(&(report.fopen_report));
      printf("fopen cost min %dms, max %dms, avg: %dms\n",report.fopen_report.min,report.fopen_report.max,report.fopen_report.avg);
      fclose_bench(&(report.fclose_report));
      printf("fclose_bench cost min %dms, max %dms, avg: %dms\n",report.fclose_report.min,report.fclose_report.max,report.fclose_report.avg);
      fwrite_bench(&(report.fwrite_report));
      printf("fwrite cost min %dms, max %dms, avg: %dms\n",report.fwrite_report.min,report.fwrite_report.max,report.fwrite_report.avg);
      fread_bench(&(report.fread_report));
      printf("fread cost min %dms, max %dms, avg: %dms\n",report.fread_report.min,report.fread_report.max,report.fread_report.avg);
      fsync_bench(&(report.fsync_report));
       printf("fsync cost fflush min %dms, max %dms, avg: %dms fileno min %dms, max %dms, avg: %dms fsync min %dms, max %dms, avg: %dms\n", \
      report.fsync_report.fflush_min,report.fsync_report.fflush_max,report.fsync_report.fflush_avg, \
      report.fsync_report.fileno_min,report.fsync_report.fileno_max,report.fsync_report.fileno_avg, \
      report.fsync_report.fsync_min,report.fsync_report.fsync_max,report.fsync_report.fsync_avg);
       sync_bench(&(report.sync_report));
       printf("sync_bench cost min %dms, max %dms, avg: %dms\n",report.sync_report.min,report.sync_report.max,report.sync_report.avg);
      //step6 remove all rw file test
      printf("start unlink file test................\n");
      sys_file_read_dir_file(MOUNT_PATH,(void *)devname,unlink_test,&(report.fsinfo_report));
      access_bench(&(report.access_report));
      unlink_bench(&(report.unlink_report));
      printf("access_bench cost min %dms, max %dms, avg: %dms\n",report.access_report.min,report.access_report.max,report.access_report.avg);
      printf("unlink_bench cost min %dms, max %dms, avg: %dms\n",report.unlink_report.min,report.unlink_report.max,report.unlink_report.avg);
     
   }else{
       printf("fsinfo cost mount %dms, statfs %dms\n",report.fsinfo_report.mount_time,report.fsinfo_report.statfs_time);
   }

if(cometfilenum > 0)
{
   printf("test create %d file\n",cometfilenum);
   for(i = 0; i < cometfilenum;i++){
       FILE_ID* handle;
       char filepath[128];
       snprintf(filepath, sizeof(filepath) - 1 ,"%s/comettest%d.mp4",MOUNT_PATH,i);
       handle = sys_libc_fopen(filepath, "w+");
       sys_libc_fclose(handle);
   }
   printf("test create file ok start read dir ......\n",cometfilenum);
   sys_file_read_dir_file(MOUNT_PATH,(void *)devname,fileshow_test,&(report.fsinfo_report));
   printf("fsinfo cost opendir: %dms readdir:%dms\n",report.fsinfo_report.opendir_time,report.fsinfo_report.readdir_time);
}

getfsinfoerror:
    sys_medium_umount(MOUNT_PATH);
mounterror:
   return 0;
}