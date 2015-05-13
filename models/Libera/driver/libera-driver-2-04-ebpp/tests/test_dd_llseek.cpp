#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <unistd.h>

#include "libera.h"

int main(int argc, char **argv)
{
    int fd1;
    //int i,j;
    //char data[30000];
	
    
    if (argc < 3) 
    {
	printf("Usage: %s timeh timel\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.dd";
    unsigned long long timeh = atoi(argv[1]);
    unsigned long timel = atoi(argv[2]);
    loff_t result;
    //timeh = 0xFFFFFFFEUL;
    //timel = 0xFFFFFFFDUL;
    unsigned long long time = (timeh << 32) | timel;
    
    printf("timeh=0x%08lx\n", (unsigned long)timeh);
    printf("timel=0x%08lx\n", timel);
    printf("time=0x%016llx\n", time);
    //goto out;
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }


    printf("Seeking SEEK_SET 64-bit fd1...\n");
    //if ( _llseek(fd1, timeh, timel, &result, SEEK_SET) < 0) {
    if ( (result = lseek(fd1, time, SEEK_CUR) < 0)) {
	perror(dev);
    }

    printf("result=0x%llx\n", result);
    //out:
    printf("Closing fd1...\n");
    close(fd1);

    return 0;
}
