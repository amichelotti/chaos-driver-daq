#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h> // memcpy()
#include <sys/select.h>

#include "libera.h"


int main(int argc, char **argv)
{
    libera_event_t le[8];
    int fd1;
    int nread;
    
    char *dev = "/dev/libera.event";
    
    printf("Opening fd1->%s in O_RDWR mode...\n", dev);
    if ((fd1 = open(dev, O_RDWR)) < 0) {
	perror("open");
	return -1;
    }

    /* Enable events */
    int mask = 0xFFFFFFFF;
    if ((ioctl(fd1, LIBERA_EVENT_SET_MASK, &mask)) < 0 ) {
	perror("LIBERA_EVENT_SET_MASK");
	close(fd1);
	return -1;
    }

    fd_set rfds;
    struct timeval tv;
    int retval;
    
    FD_ZERO(&rfds);
    FD_SET(fd1, &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    
    retval = select(fd1 + 1, &rfds, NULL, NULL, &tv);
    
    if (retval) {
	size_t count = 0; 
        if (-1 == ioctl(fd1, FIONREAD, &count)) { 
	    perror("ioctl FIONREAD"); 
	    return -1; 
        } 
	printf("%d bytes queued: ", count); 

	if ((nread = read(fd1, le, 5*sizeof(libera_event_t))) < 0) {
	    perror("read");
	    close(fd1);
	    return -1;
	}
	printf("Received %d bytes (%d event(s)).\n", 
	       nread, nread/sizeof(libera_event_t));
	if ( le[0].id == LIBERA_EVENT_TRIGGET )
	    printf("EVENT: Libera GET trigger.\n");
    }
    else
	printf("No data within five seconds.\n");

    /* Disable events */
    mask = 0;
    if ((ioctl(fd1, LIBERA_EVENT_SET_MASK, &mask)) < 0 ) {
	perror("LIBERA_EVENT_SET_MASK");
	close(fd1);
	return -1;
    }


    printf("Closing fd1...\n");
    close(fd1);
    
    
    return 0;
}
