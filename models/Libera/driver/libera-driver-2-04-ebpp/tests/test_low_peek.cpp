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
    int fd;
    
    
    char *dev = "/dev/libera.low";
    libera_peek_poke_t peek;

    if (argc < 2) 
    {
	printf("Usage: %s offset\n", argv[0]);
	return(-1);
    }
    peek.offset = atol(argv[1]);
    
    if ((fd = open(dev, O_RDWR)) < 0) {
	perror(dev);
	return -1;
    }

    if ((ioctl(fd, LIBERA_EVENT_PEEK, &peek)) < 0) {
	perror(dev);
	return -1;
    }

    // Report the catch...
    printf("0x%lx\n", peek.value);
    fflush(stdout);

    close(fd);
    return 0;
}
