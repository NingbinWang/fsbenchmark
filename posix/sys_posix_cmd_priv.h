#ifndef _SYS_POSIX_CMD_PRIV_H_
#define _SYS_POSIX_CMD_PRIV_H_

#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/if.h>   
#include <linux/if_ether.h>   
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stddef.h>
#include"sys_posix.h"

#define SYS_SOL_SOCKET      1

#define SYS_SOCKET_SAFE_CLOSE(x) \
do { \
    if (-1 != (x)) \
    { \
        (VOID)sys_socket_close((x)); \
        (x) = -1; \
    } \
}while (0)


#define CMD_BUF_MAX_LEN			1024

/* 命令头部 */


#endif