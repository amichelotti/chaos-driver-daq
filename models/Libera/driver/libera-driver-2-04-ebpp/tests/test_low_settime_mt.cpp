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
    int fd1;
    
    char *dev = "/dev/libera.event";

    if (argc < 2)
      {
        printf("Usage: %s mt\n", argv[0]);
        return(-1);
      }

    unsigned long long mt = atol(argv[1]);
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror("open");
	return -1;
    }

  
    printf("Setting MT to %llx\n", mt);
    if ((ioctl(fd1, LIBERA_EVENT_SET_MT, &mt)) < 0 ) {
	perror("LIBERA_EVENT_SET_MT");
	close(fd1);
	return -1;
    }


    printf("Closing fd1...\n");
    close(fd1);
    
    
    return 0;
}
