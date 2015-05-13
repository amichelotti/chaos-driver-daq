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
    libera_peek_poke_t poke;

    if (argc < 3) 
    {
	printf("Usage: %s offset value \n", argv[0]);
	return(-1);
    }
    poke.offset = atol(argv[1]);
    poke.value = atol(argv[2]);
    
    if ((fd = open(dev, O_RDWR)) < 0) {
	perror(dev);
	return -1;
    }

    if ((ioctl(fd, LIBERA_EVENT_POKE, &poke)) < 0) {
	perror(dev);
	return -1;
    }

    close(fd);
    return 0;
}
