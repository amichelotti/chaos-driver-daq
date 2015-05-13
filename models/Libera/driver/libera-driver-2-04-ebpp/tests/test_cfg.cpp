#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "libera.h"


inline void kbpause(int seconds)
{
    for(int i=seconds; i>0; i--) 
    {
	//printf("%d ", i);
	//fflush(stdout);
	sleep(1);
    }
    //printf("\n\n");
    
}

inline void report(libera_cfg_request_t param)
{
    printf("param(%u)=%u\n", param.idx, param.val);
}



int main(int argc, char **argv)
{
    int fd1, fd2;
    libera_cfg_request_t param;
    param.idx = LIBERA_CFG_KX;

    if (argc < 2) 
    {
	printf("Usage: %s pause_interval\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.cfg";
    int delay = atoi(argv[1]);
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }
    
    //kbpause(delay);
    printf("Opening fd2->%s in O_RDWR mode...\n", dev);
    if ((fd2 = open(dev, O_RDWR)) < 0) {
	perror(dev);
	return -1;
    }


    kbpause(delay);
    printf("Calling ioctl GET on fd1...\n");
    if ( ioctl(fd1, LIBERA_IOC_GET_CFG, &param) < 0) {
	perror(dev);
    }
    report(param);

    kbpause(delay);
    printf("Calling ioctl SET on fd1... (%d)\n", ++param.val);
    if ( ioctl(fd1, LIBERA_IOC_SET_CFG, &param) < 0) {
	perror(dev);
    }

    kbpause(delay);
    printf("Calling ioctl GET on fd1...\n");
    if ( ioctl(fd1, LIBERA_IOC_GET_CFG, &param) < 0) {
	perror(dev);
    }
    report(param);



    kbpause(delay);
    printf("Calling ioctl GET on fd2...\n");
    if ( ioctl(fd2, LIBERA_IOC_GET_CFG, &param) < 0) {
	perror(dev);
    }
    report(param);

    kbpause(delay);
    printf("Calling ioctl SET on fd2... (%d)\n", ++param.val);
    if ( ioctl(fd2, LIBERA_IOC_SET_CFG, &param) < 0) {
	perror(dev);
    }

    kbpause(delay);
    printf("Calling ioctl GET on fd2...\n");
    if ( ioctl(fd2, LIBERA_IOC_GET_CFG, &param) < 0) {
	perror(dev);
    }
    report(param);

    // NOTE: LIBERA_IOC_GET_CFG+1 is actually a SET_ method LIBERA_IOC_SET_CFG
    kbpause(delay);
    printf("Calling invalid ioctl on fd1...\n");
    if ( ioctl(fd1, (LIBERA_IOC_GET_CFG+100), &param) < 0) {
	perror(dev);
    }
    report(param);

    kbpause(delay);
    printf("Calling invalid ioctl on fd2...\n");
    if ( ioctl(fd2, (LIBERA_IOC_GET_CFG+100), &param) < 0) {
	perror(dev);
    }
    report(param);


    kbpause(delay);
    param.val = 3900000000UL;
    printf("Calling ioctl SET on fd2...\n");
    if ( ioctl(fd2, LIBERA_IOC_SET_CFG, &param) < 0) {
	perror(dev);
    }
    
    kbpause(delay);
    printf("Calling ioctl GET on fd2...\n");
    if ( ioctl(fd2, LIBERA_IOC_GET_CFG, &param) < 0) {
	perror(dev);
    }
    report(param);

    //kbpause(delay);
    //kbpause(delay);
    printf("Closing fd1...\n");
    close(fd1);

    //kbpause(delay);
    printf("Closing fd2...\n");
    close(fd2);


    return 0;
}
