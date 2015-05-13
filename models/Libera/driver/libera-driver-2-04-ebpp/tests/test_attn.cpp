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
    
    
    if (argc < 9) 
    {
	printf("Usage: %s att0 att1 .. att7\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.cfg";
    int atti[8];
    unsigned char attc[8];
    int i;
    

    for(i=0; i<8; i++)
    {
	atti[i]= atoi(argv[i+1]);
	attc[i] = atti[i];
    }
    
    
    printf("Opening fd1->%s in O_RDWR mode...\n", dev);
    if ((fd1 = open(dev, O_RDWR)) < 0) {
	perror(dev);
	return -1;
    }

    printf("Setting attenuators to:\n");
    for(i=0; i<8; i++)
    {
	printf("0x%x\n", attc[i]);
    }
    if ( ioctl(fd1, LIBERA_IOC_SET_ATTN, attc) < 0) {
	perror(dev);
    }

    if ( ioctl(fd1, LIBERA_IOC_GET_ATTN, attc) < 0) {
	perror(dev);
    }
    printf("Driver readback:\n");
    for(i=0; i<8; i++)
    {
	printf("0x%x\n", attc[i]);
    }
    
    printf("Closing fd1...\n");
    close(fd1);


    return 0;
}
