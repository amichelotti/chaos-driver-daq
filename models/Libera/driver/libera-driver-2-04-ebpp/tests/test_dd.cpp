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


int main(int argc, char **argv)
{
    int fd1, fd2;
    
    
    if (argc < 2) 
    {
	printf("Usage: %s pause_interval\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.dd";
    int delay = atoi(argv[1]);
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }
    
    kbpause(delay);
    printf("Opening fd2->%s in O_RDONLY mode...\n", dev);
    if ((fd2 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }

    char data[3000];

    kbpause(delay);
    printf("Calling read on fd1...\n");
    if ( read(fd1, &data, sizeof(libera_atom_dd_t)) < 0) {
	perror(dev);
    }

    goto out;
    kbpause(delay);
    printf("Seeking to MT=0 on fd1...\n");
    if ( lseek(fd1, 0, SEEK_CUR) < 0) {
	perror(dev);
    }

    kbpause(delay);
    printf("Calling read on fd1...\n");
    if ( read(fd1, &data, sizeof(libera_atom_dd_t)) < 0) {
	perror(dev);
    }


    
 out:
    kbpause(delay);
    kbpause(delay);
    printf("Closing fd1...\n");
    close(fd1);

    kbpause(delay);
    printf("Closing fd2...\n");
    close(fd2);


    return 0;
}
