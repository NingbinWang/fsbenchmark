#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <sys/time.h> 
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include "utils.h"

VOID sys_bubble_sort(INT arr[], INT len)
{
    int i, j, temp;
    for (i = 0; i < len - 1; i++)
        for (j = 0; j < len - 1 - i; j++)
            if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
}