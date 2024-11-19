#ifndef _FAT_FS_H_
#define _FAT_FS_H_

int make_fat_fs(const char *dev_name, int sectors_per_cluster, int sect_size, int fat_size, int volume_name);

int fdisk_make_part(const char *dev_name, unsigned int parts_num);

#endif
