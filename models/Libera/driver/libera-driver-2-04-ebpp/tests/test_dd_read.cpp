#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "libera.h"

#define MAX_PAGES 4095

int main(int argc, char **argv)
{
    int fd1;
    //register unsigned int i,j;
    char *data;
    ssize_t ret;
    ssize_t req_size;

    if (argc < 4) 
    {
	printf("Usage: %s mt_offset atoms decimation\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.dd";
    unsigned long long mt_offset = atoi(argv[1]);
    unsigned int size = atoi(argv[2]);
    unsigned long dec = atoi(argv[3]);

    data = (char *)malloc(size*32);
    
    if (!data) {
	fprintf(stderr, "Cannot allocate buffer.\n");
	return -1;
    }
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }

    printf("Setting decimation = %lu\n", dec);
    if ( ioctl(fd1, LIBERA_IOC_SET_DEC, &dec) < 0) {
	perror(dev);
	return -1;
    }

    printf("Seeking to MT=%llu on fd1...\n", mt_offset);
    if ( lseek(fd1, mt_offset, SEEK_CUR) < 0) {
	perror(dev);
	return -1;
    }

    req_size = size*sizeof(libera_atom_dd_t);
    printf("Reading 0x%x DD atoms (%u bytes) from fd1...\n",
	   size, size*sizeof(libera_atom_dd_t));
    ret = read(fd1, data, req_size);
    if ( ret != req_size) {
	if ( ret < 0) {
	    perror(dev);
	    return -1;
	}
	else {
	    printf("\nOVERRUN!!! Read only %d bytes.\n\n", ret);
	}
    }

#if 0
    libera_timestamp_t *tstamp = (libera_timestamp_t *)&data[0];
    libera_atom_dd_t *dd_data = (libera_atom_dd_t *)
	&data[sizeof(libera_timestamp_t)];
    
    printf("\nTimestamp (MT,STs,STns)= (%llu,%lu,%lu)\n", 
	   tstamp->mt, tstamp->st.tv_sec, tstamp->st.tv_nsec);

    printf("Data:\n");
    for (i=0; i< size;i++)
    {
	printf("Atom %2d: ", i);
	for (j=0;j<sizeof(libera_atom_dd_t);j++)
	{
	    
	    printf("%2x ", *((char *)dd_data + (i*sizeof(libera_atom_dd_t)) +j));
	}
	printf("\n");
    }
#endif
    
    printf("Closing fd1...\n");
    close(fd1);
    if (data) free(data);

    return 0;
}
