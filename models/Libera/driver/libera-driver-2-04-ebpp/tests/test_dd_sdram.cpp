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
    unsigned int i,j;
    char data[4096*1024+sizeof(libera_timestamp_t)];
	
    
    if (argc < 2) 
    {
	printf("Usage: %s atoms\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.dd";
    unsigned int size = atoi(argv[1]);
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }


    printf("Seeking to MT=0 on fd1...\n");
    if ( lseek(fd1, 0, SEEK_CUR) < 0) {
	perror(dev);
    }

    printf("Reading 0x%x DD atoms (%u bytes) from fd1...\n",
	   size, size*sizeof(libera_atom_dd_t));
    if ( read(fd1, (void *)data, size*sizeof(libera_atom_dd_t)
	      + sizeof(libera_timestamp_t)) < 0) {
	perror(dev);
    }

    libera_timestamp_t *tstamp = (libera_timestamp_t *)data;
    libera_atom_dd_t *dd_data = (libera_atom_dd_t *)
	&data[sizeof(libera_timestamp_t)];
    printf("\nTimestamp (MT,STs,STns)= (%llu,%lu,%lu)\n", 
	   tstamp->mt, tstamp->st.tv_sec, tstamp->st.tv_nsec);

    //goto out;

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

    
    // out:
    printf("Closing fd1...\n");
    close(fd1);

    return 0;
}
