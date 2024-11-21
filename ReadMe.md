# FileSystem benchmark
这个主要是用于测试文件系统相关的性能数据，主要看时间差进行对比。

## use
  make all CROSS_COMPILE=gcc
在output下产生fsbench
命令：
   ./fsbench -d /dev/mmcblk0p1 -j 6 -c 9000
   6路线程写数据sync后再读数据 重命名 删除文件 最后生产9000个文件 进行读文件夹数据并删除

* -v | --version       show version\n"
* -h | --help          Print this message\n"
* -d | --devname [arg] devname; like /dev/mmblk0p1\n"
* -j | --jobs           read/write pthread 1~64\n"
* -p | --bypass           don't test open read write sync api \n"
* -c | --comet [arg] filenumber; like 9000  create 9000 files and readdir\n"
* -l | --lenght [arg] perfilesize(/MB); like 270  perfile size\n"
* -s | --prerwsize [arg] perrwsize(/KB); like 512  perfile size\n"

## API test

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
1. 线程变成乱序方式写入  完成
2. 每个文件的大小可以被修改 完成
3. 生成CVS的文档，把所有的数据存起来