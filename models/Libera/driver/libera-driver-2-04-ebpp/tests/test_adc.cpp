#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "libera.h"


int main(int argc, char **argv)
{
    int fd1;
    register unsigned int i;
    libera_atom_adc_t *data;
    ssize_t ret;
    ssize_t req_size;

    if (argc < 2) 
    {
	printf("Usage: %s atoms \n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.adc";
    unsigned int size = atoi(argv[1]);

    data = (libera_atom_adc_t *)malloc(size*sizeof(libera_atom_adc_t));
    
    if (!data) {
        fprintf(stderr, "Cannot allocate buffer.\n");
	return -1;
    }
    
    //printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }

    req_size = size*sizeof(libera_atom_adc_t);
    //printf("Reading 0x%x DD atoms (%u bytes) from fd1...\n",
    //	   size, req_size);
    ret = read(fd1, data, req_size);
    if ( ret != req_size) {
	if ( ret < 0) {
	    perror(dev);
	    return -1;
	}
    }

    for (i=0; i< size;i++)
    {
      //printf("Atom %2d: ", i);
      printf("%8d %8d %8d %8d\n", 
	     data[i].ChA,
	     data[i].ChB,
	     data[i].ChC,
	     data[i].ChD);
    }
    
    //printf("Closing fd1...\n");
    close(fd1);
    if (data) free(data);

    return 0;
}
