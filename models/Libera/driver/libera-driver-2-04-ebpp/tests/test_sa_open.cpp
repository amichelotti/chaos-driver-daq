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
    
    if (argc < 2) 
    {
	printf("Usage: %s number_of_reading_clients\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.sa";
    int enable_r = atoi(argv[1]);

    int fd[enable_r];  // array of file descriptors
    
    // Register as notofocation recipient
    printf("\nOpening %s in O_RDONLY mode... %d  times \n \n", dev,enable_r);

	for (int i=0;i<enable_r;i++)
	{
    	
		if ((fd[i] = open(dev, O_RDONLY)) < 0) {
			perror(dev);
			return -1;
		}
	}
    
    printf("Closing fd...\n");

	for (int i=0;i<enable_r;i++)
	{
    	close(fd[i]);
	}

    return 0;
}
