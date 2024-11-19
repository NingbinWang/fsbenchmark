
#ifndef _SYS_MEDIUM_PRIV_H_
#define _SYS_MEDIUM_PRIV_H_
#include <sys/mount.h>
#include <sys/types.h>
#include <unistd.h>
#include"sys_common.h"
#include"sys_medium.h"
#include"sys_posix.h"
#include"sys_libc_file.h"
#include"sys_time.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/vfs.h>    /* or <sys/statfs.h> */
#include "mkfs.h"


typedef enum  SYS_MEDIUM_FS_MAGIC_E
{
    
    SYS_MEDIUM_FS_MAGIC_FAT32 = 0x4d44,
    SYS_MEDIUM_FS_MAGIC_EXT4  = 0xef53,
    SYS_MEDIUM_FS_MAGIC_EXFAT = 0x65735546,
} SYS_MEDIUM_FS_MAGIC_E;

#endif