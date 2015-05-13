#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h> // memcpy()
#include <unistd.h> // usleep();

#include "libera.h"

inline void kbpause(int seconds)
{
    for(int i=seconds; i>0; i--) 
    {
	printf("%d ", i);
	fflush(stdout);
	sleep(1);
    }
    printf("\n\n");
    
}

inline void report(int param)
{
    printf("answer=%d\n", param);
}




int main(int argc, char **argv)
{
    int fd1, fd2;
    //int param;
    
    
    if (argc < 2) 
    {
	printf("Usage: %s sample_time_ms\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.sa";
    int delay = atoi(argv[1]);
    //int ret;

   
    
    printf("Opening fd1->%s in O_RDWR mode...\n", dev);
    if ((fd1 = open(dev, O_RDWR)) < 0) {
	perror(dev);
	return -1;
    }


    while (true)
    {
	static int count;
	
	printf("Calling write on fd1... %d\n", count);
	if ((write(fd1, &(count), sizeof(int))) < 0) {
	    perror(dev);
	    return -1;
	}
	count++;
	usleep(1000*delay); // 100 ms ~ 10Hz
	
    }
    

    
    kbpause(delay);
    printf("Closing fd1...\n");
    close(fd1);

    kbpause(delay);
    printf("Closing fd2...\n");
    close(fd2);


    return 0;
}
