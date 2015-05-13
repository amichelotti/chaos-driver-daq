#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h> // memcpy()

#include "libera.h"


int main(int argc, char **argv)
{
    int fd1;
    
    
    char *dev = "/dev/libera.event";
    libera_Ltimestamp_t ctime;
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }

    if ((ioctl(fd1, LIBERA_EVENT_GET_CTIME, &ctime)) < 0) {
	perror(dev);
	return -1;
    }

    // Report the catch...
    printf("Read ctime (LST, LMT): (%llu, %llu)\n",
	   ctime.lst,
	   ctime.lmt);
    fflush(stdout);

    printf("Closing fd1...\n");
    close(fd1);
    
    
    return 0;
}
