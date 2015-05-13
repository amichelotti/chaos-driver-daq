#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <fcntl.h>

#include "libera.h"

#define MAX_PAGES 4095

int main(int argc, char **argv)
{
    const static size_t buf_samples = 8*1024;
    const static size_t buf_size = buf_samples * sizeof(libera_sample_t);

    if (argc < 4) 
    {
	printf("Usage: %s mt_offset samples decimation [samples_per_atom]\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.dd";
    off_t mt_offset = atoll(argv[1]);
    unsigned int size = atoi(argv[2]);
    unsigned long dec = atoi(argv[3]);
    unsigned long per_atom = 4;

    if (argc > 4) {
    	per_atom = atoi(argv[4]);
    }

    libera_sample_t *data = (libera_sample_t *)malloc(buf_size);
    
    if (!data) {
	fprintf(stderr, "Cannot allocate buffer.\n");
	return -1;
    }
    
    int fd1;
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }

    //printf("Setting decimation = %lu\n", dec);
    if ( ioctl(fd1, LIBERA_IOC_SET_DEC, &dec) < 0) {
	perror(dev);
	return -1;
    }

    //printf("Seeking to MT=%llu on fd1...\n", mt_offset);
    if ( lseek(fd1, mt_offset, SEEK_CUR) < 0) {
	perror(dev);
	return -1;
    }

    size_t i = 0;
    ssize_t ret = 0;
    for (size_t r = size*sizeof(libera_sample_t); r > 0; r -= ret) {
	ssize_t req_size = MIN(buf_size, r);
        ret = read(fd1, data, req_size);
        if ( ret != req_size) {
            if ( ret < 0) {
               perror(dev);
               break;
            }
            else {
               printf("\nWARNING: read() returned %u instead of %u.\n\n", ret, req_size);
               break;
            }
        }
	if (per_atom > 0) {
            for (ssize_t bi = 0; bi < ret/sizeof(libera_sample_t); ++bi, ++i) {
                printf("%d ", data[bi]);
                if (!((i+1) % per_atom)) printf("\n");
            }
        }
    }
    
    close(fd1);
    if (data) free(data);

    return 0;
}
