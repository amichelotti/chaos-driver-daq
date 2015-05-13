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
    
    
    if (argc < 2)
      {
        printf("Usage: %s trigger_No \n", argv[0]);
        return(-1);
      }
    int trigger_no = atoi(argv[1]);

    char *dev = "/dev/libera.event";
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }

    if ((ioctl(fd1, LIBERA_EVENT_ENABLE_MC_TRIG, 1<<trigger_no)) < 0) {
      perror(dev);
      return -1;
    }

    printf("Enabled MC trigger %u\n", trigger_no);
    fflush(stdout);

    printf("Closing fd1...\n");
    close(fd1);
    
    
    return 0;
}
