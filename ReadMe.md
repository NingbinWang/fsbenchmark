# FileSystem benchmark
这个主要是用于测试文件系统相关的性能数据，主要看时间差进行对比。

# Getting Started Quickly

请修改Makefile中的TARGET_CROSS_HOST，指定交叉编译链

> make all

在output下产生fsbench

注意目前已经验证的文件系统：vfat 

命令：

```
   ./fsbench -d /dev/mmcblk0p1 -j 6 -c 9000
```

说明：6路线程写数据sync后再读数据 重命名 删除文件 最后生产9000个文件 进行读文件夹数据并删除 

## 具体参数说明

* -v | --version       show version 打印版本信息
* -h | --help          Print this message 打印帮助信息
* -d | --devname [arg] devname; like /dev/mmblk0p1 设置文件系统需要挂载的路径
* -j | --jobs           read/write pthread 1~64  开多少个线程来读写
* -p | --bypass           don't test open read write sync api 跳过创建线程并进行读写测试
* -c | --comet [arg] filenumber; like 9000  create 9000 files and readdir  创建多少个文件并进行读取
* -l | --lenght [arg] perfilesize(/MB); like 270  perfile size     每个文件最大的size
* -s | --prerwsize [arg] perrwsize(/KB); like 512  perfile size    每次读写的最大的size
* -f | --outfile [arg] csvfilename; like fsbench.csv 输出整个测试过程中的相应的实时数据

## 修改测试记录数据的大小

目前测试数据主要在inc/report.h中记录：

```
#define MAX_FOPEN    64*1024
#define MAX_FCLOSE   64*1024
#define MAX_FREAD   64*1024
#define MAX_FWRITE  64*1024
#define MAX_FSYNC   64*1024
#define MAX_FSTAT   64*1024
#define MAX_SYNC    64*1024

#define MAX_FILENUM 20000
```

这里的宏定义就是表明整个测试过程中可以被记录的最大数

## 目前测试的API有

* mount
* statfs
* fopen
* fclose
* fread
* fwrite
* opendir
* readdir
* rename
* fstat
* fsync
* sync
* unlink


# TODO
1. 线程变成乱序方式写入(完成)
2. 每个文件的大小可以被修改(完成)
3. 生成CVS的文档，把所有的数据存起来(完成)
4. 对于open，read，write等unix系统级的接口做测试 
5. 解决所有的编译waring