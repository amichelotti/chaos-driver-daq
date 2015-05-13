#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "libera.h"




int main(int argc, char **argv)
{
    int fd1;
    //int fd2;
    //int param;
    
    
    if (argc < 2) 
    {
	printf("Usage: %s switch_code\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.cfg";
    int Switch = atoi(argv[1]);
    
    printf("Opening fd1->%s in O_RDWR Switch...\n", dev);
    if ((fd1 = open(dev, O_RDWR)) < 0) {
	perror(dev);
	return -1;
    }

    printf("Setting Switch  to: 0x%x\n", Switch);
    if ( ioctl(fd1, LIBERA_IOC_SET_SWITCH, &Switch) < 0) {
	perror(dev);
    }

    if ( ioctl(fd1, LIBERA_IOC_GET_SWITCH, &Switch) < 0) {
	perror(dev);
    }
    printf("Driver readback: Switch = 0x%x\n", Switch);
    
    printf("Closing fd1...\n");
    close(fd1);


    return 0;
}
