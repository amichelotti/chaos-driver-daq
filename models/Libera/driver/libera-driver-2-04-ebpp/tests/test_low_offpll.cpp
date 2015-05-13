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
        printf("Usage: %s LMT_PLL_offset\n", argv[0]);
        return(-1);
      }

    unsigned long long off_pll = atol(argv[1]);
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror("open");
	return -1;
    }

  
    printf("Setting LMT off_pll to 0x%llx\n", off_pll);
    if ((ioctl(fd1, LIBERA_EVENT_SET_OFFPLL, &off_pll)) < 0 ) {
	perror("LIBERA_EVENT_SET_OFFPLL");
	close(fd1);
	return -1;
    }


    printf("Closing fd1...\n");
    close(fd1);
    
    
    return 0;
}
