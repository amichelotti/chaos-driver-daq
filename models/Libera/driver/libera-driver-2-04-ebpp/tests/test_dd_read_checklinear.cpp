#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "libera.h"


/* test_dd_read_linear.cpp 
 *
 * This test is designed for testing the correctness
 * of data transfer when SDRAM is filled with "linear data", 
 * that is, with the byte values: 0 0 0 1 0 0 0 2 0 0 0 3 ...
 */

int main(int argc, char **argv)
{
    int fd1;
    register unsigned int i;
    char *data;
    ssize_t ret;
    ssize_t req_size;
    int corrupted = 0;

    if (argc < 4) 
    {
	printf("Usage: %s mt_offset atoms decimation\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.dd";
    unsigned long long mt_offset = atoi(argv[1]);
    unsigned int size = atoi(argv[2]);
    unsigned long dec = atoi(argv[3]);

    data = (char *)malloc(size*32+sizeof(libera_timestamp_t));
    
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

    req_size = sizeof(libera_timestamp_t) +
	size*sizeof(libera_atom_dd_t);
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

    libera_atom_dd_t *dd_data = (libera_atom_dd_t *)
	&data[sizeof(libera_timestamp_t)];

    /* Check linear data */ 
    for (i=1; i< (ret-sizeof(libera_timestamp_t))/sizeof(unsigned long);i++)
    {
	unsigned long longword = *((unsigned long *)dd_data + (i-1));
	unsigned long longword_next = *((unsigned long *)dd_data + i);
	unsigned long swapped;
	unsigned long swapped_next;
	char * longwordp = (char *)&longword;
	char * longword_nextp = (char *)&longword_next;
	char * swappedp  = (char *)&swapped;
	char * swapped_nextp  = (char *)&swapped_next;

	/* swap bytes */
	*(swappedp + 0) = *(longwordp + 3);
	*(swappedp + 1) = *(longwordp + 2);
	*(swappedp + 2) = *(longwordp + 1);
	*(swappedp + 3) = *(longwordp + 0);

	*(swapped_nextp + 0) = *(longword_nextp + 3);
	*(swapped_nextp + 1) = *(longword_nextp + 2);
	*(swapped_nextp + 2) = *(longword_nextp + 1);
	*(swapped_nextp + 3) = *(longword_nextp + 0);

	if ((swapped + 1) != swapped_next) {
	    printf("!!!!!! Data corruption at longword #%u !!!!!!\n", i);
	    corrupted = 1;
	    break;
	}
    }
    if (!corrupted) printf("Linear data OK.\n");
    
    printf("Closing fd1...\n");
    close(fd1);
    if (data) free(data);

    return 0;
}
