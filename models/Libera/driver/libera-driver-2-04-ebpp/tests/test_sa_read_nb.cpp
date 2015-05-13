#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h> // memcpy()
#include <errno.h>

#include "libera.h"


int main(int argc, char **argv)
{
    int fd1;
    int count = 0;
    int ret;

    if (argc < 2) 
    {
	printf("Usage: %s enable_reading\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.sa";
    int enable_r = atoi(argv[1]);
    //int ret;
    libera_atom_sa_t libera_sa_sample;
    
    // Register as notofocation recipient
    //printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY | O_NONBLOCK)) < 0) {
	perror(dev);
	return -1;
    }

    if (enable_r)
    {
	
	while (true)
	{
            do {
                ret = read(fd1, &libera_sa_sample, 
                               sizeof(libera_atom_sa_t));
                if (ret < 0) {
                    if (errno == EAGAIN) {
                        count++;
                        continue;
                    }
                    perror(dev);
                    return -1;
                }
                
                printf("%d reads in between.\n", count);
                count = 0;
                
                // Report the catch...
                printf("%d   %d   %d   %d   %d   %d   %d   %d\n",
                       libera_sa_sample.Va,
                       libera_sa_sample.Vb,
                       libera_sa_sample.Vc,
                       libera_sa_sample.Vd,
                       libera_sa_sample.X,
                       libera_sa_sample.Y,
                       libera_sa_sample.Q,
                       libera_sa_sample.Sum);
                fflush(stdout);
            } while (ret > 0);
            sleep(1);
        }
    }
        
    printf("Closing fd1...\n");
    close(fd1);


    return 0;
}
