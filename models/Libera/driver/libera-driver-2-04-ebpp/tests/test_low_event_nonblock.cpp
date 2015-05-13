#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h> // memcpy()
#include <sys/select.h>

#include "libera.h"


int main(int argc, char **argv)
{
    libera_event_t le;
    int fd1;
    
    
    char *dev = "/dev/libera.event";
    
    printf("Opening fd1->%s in O_RDWR mode...\n", dev);
    if ((fd1 = open(dev, O_RDWR | O_NONBLOCK)) < 0) {
	perror("open");
	return -1;
    }

    /* Enable events */
    int mask = 0xFFFFFFFF;
    if ((ioctl(fd1, LIBERA_EVENT_SET_MASK, &mask)) < 0 ) {
	perror("LIBERA_EVENT_SET_MASK");
	close(fd1);
	return -1;
    }

    if ((read(fd1, &le, sizeof(le))) < 0) {
	perror("read");
	close(fd1);
	return -1;
    }

    /* Disable events */
    mask = 0;
    if ((ioctl(fd1, LIBERA_EVENT_SET_MASK, &mask)) < 0 ) {
	perror("LIBERA_EVENT_SET_MASK");
	close(fd1);
	return -1;
    }


    printf("Closing fd1...\n");
    close(fd1);
    
    
    return 0;
}
