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

#define I_MAX 100000
libera_Ltimestamp_t myctime[I_MAX];


int main(int argc, char **argv)
{
    int fd1;
    int i;

    if (argc < 3)
      {
        printf("Usage: %s ascii_filename bin_filename\n", argv[0]);
        return(-1);
      }
    char *ascii_filename = argv[1];
    char *bin_filename = argv[2];

    
    char *dev = "/dev/libera.event";
    
    printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }

    for (i=0; i<I_MAX; i++)
      {
	if ((ioctl(fd1, LIBERA_EVENT_GET_CTIME, &myctime[i])) < 0) {
	  perror(dev);
	  return -1;
	}
      }

    FILE *asciifp = NULL;
    FILE *binfp = NULL;

    asciifp = fopen( ascii_filename, "w" );
    binfp = fopen( bin_filename, "w" );
    if (( 0 == asciifp ) || (0 == binfp))
	return -1;

    for (i=0; i<I_MAX; i++)
      {
	if (fwrite(&myctime[i], 
		   sizeof(libera_timestamp_t), 1, binfp) != 1)
	  {
	    printf("Error writing bin file\n");
	    fclose (binfp);
	    return -1;
	  }
	fprintf(asciifp, "%llu    %llu\n", myctime[i].lst, myctime[i].lmt);
      }
    
    fclose (binfp);
    fclose (asciifp);

    printf("Closing fd1...\n");
    close(fd1);
    
    
    return 0;
}
