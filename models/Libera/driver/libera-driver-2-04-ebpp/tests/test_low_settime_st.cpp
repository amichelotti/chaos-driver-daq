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

    if (argc < 3)
      {
        printf("Usage: %s st.tv_sec st.tv_nsec\n", argv[0]);
        return(-1);
      }

    struct timespec st;

    st.tv_sec = atoi(argv[1]);
    st.tv_nsec = atoi(argv[2]);
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror("open");
	return -1;
    }

  
    printf("Setting ST to %lu s %lu ns\n", st.tv_sec, st.tv_nsec);
    if ((ioctl(fd1, LIBERA_EVENT_SET_ST, &st)) < 0 ) {
	perror("LIBERA_EVENT_SET_ST");
	close(fd1);
	return -1;
    }


    printf("Closing fd1...\n");
    close(fd1);
    
    
    return 0;
}
