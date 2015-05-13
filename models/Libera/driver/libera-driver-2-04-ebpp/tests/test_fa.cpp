#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "libera.h"

inline void kbpause(int seconds)
{
    for(int i=seconds; i>0; i--) 
    {
	//printf("%d ", i);
	//fflush(stdout);
	sleep(1);
    }
    //printf("\n\n");
    
}

inline void report(int param)
{
    printf("answer=%d\n", param);
}


int main(int argc, char **argv)
{
    int fd1, fd2;
    
    
    if (argc < 2) 
    {
	printf("Usage: %s pause_interval\n", argv[0]);
	return(-1);
    }
    
    char *dev = "/dev/libera.fa";
    int delay = atoi(argv[1]);

    
    printf("Opening fd1->%s in O_RDWR mode...\n", dev);
    if ((fd1 = open(dev, O_RDWR)) < 0) {
	perror(dev);
	return -1;
    }
    
    kbpause(delay);
    printf("Opening fd2->%s in O_RDWR mode...\n", dev);
    if ((fd2 = open(dev, O_RDWR)) < 0) {
	perror(dev);
	// This will fail, therefore continue...
	//return -1;
    }

    kbpause(delay);
    printf("Opening fd2->%s in O_RDONLY mode...\n", dev);
    if ((fd2 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
	return -1;
    }

    char buf[1024];
    int count = 4;
    int my_value[4] = {1, 20, 300, 4000};
    int ret, read_value;
    
    kbpause(delay);
    printf("Calling read on fd1...\n");
    if ( read(fd1, buf, count) < 0) {
	perror(dev);
    }
    buf[4]='\0';
    report(atoi(buf));

    kbpause(delay);
    printf("Seeking SEEK_CURR -4L on fd1...\n");
    if ( (ret = lseek(fd1, -4L, SEEK_CUR)) < 0) {
	perror(dev);
    }
    printf("lseek() returned %d\n", ret);

    kbpause(delay);
    printf("Seeking SEEK_SET -8L on fd1...\n");
    if ( (ret = lseek(fd1, -8L, SEEK_SET)) < 0) {
	perror(dev);
    }
    printf("lseek() returned %d\n", ret);

    kbpause(delay);
    printf("Seeking SEEK_SET 20L on fd1...\n");
    if ( (ret = lseek(fd1, 20L, SEEK_SET)) < 0) {
	perror(dev);
    }
    printf("lseek() returned %d\n", ret);


    kbpause(delay);
    printf("Seeking SEEK_END -4L on fd1...\n");
    if ( (ret = lseek(fd1, -4L, SEEK_END)) < 0) {
	perror(dev);
    }
    printf("lseek() returned %d\n", ret);

    kbpause(delay);
    printf("Seeking SEEK_END 0L on fd1...\n");
    if ( (ret = lseek(fd1, 0L, SEEK_END)) < 0) {
	perror(dev);
    }
    printf("lseek() returned %d\n", ret);

    kbpause(delay);
    printf("Seeking SEEK_END 4L on fd1...\n");
    if ( (ret = lseek(fd1, 4L, SEEK_END)) < 0) {
	perror(dev);
    }
    printf("lseek() returned %d\n", ret);

    kbpause(delay);
    printf("Seeking SEEK_SET 0L on fd1...\n");
    if ( (ret = lseek(fd1, 0L, SEEK_SET)) < 0) {
	perror(dev);
    }
    printf("lseek() returned %d\n", ret);


    kbpause(delay);
    printf("Calling write on fd1...\n");
    if ( (ret = write(fd1, &my_value, sizeof(my_value))) < 0) {
	perror(dev);
    }
    printf("write() returned %d\n", ret);

    kbpause(delay);
    printf("Calling write on fd1 (over the buffer)...\n");
    if ( (ret = write(fd1, &my_value, sizeof(my_value))) < 0) {
	perror(dev);
    }
    printf("write() returned %d\n", ret);

    kbpause(delay);
    printf("Calling read on fd1 (over the buffer)...\n");
    if ( (ret = read(fd1, &my_value, sizeof(my_value))) < 0) {
	perror(dev);
    }
    printf("read() returned %d\n", ret);
    
    kbpause(delay);
    printf("Seeking SEEK_SET 0L on fd1...\n");
    if ( (ret = lseek(fd1, 0L, SEEK_SET)) < 0) {
	perror(dev);
    }
    printf("lseek() returned %d\n", ret);
    
    for(int i=0; i< 3; i++)
    {
	kbpause(delay);
	printf("Calling read on fd1...\n");
	if ( read(fd1, &read_value, count) < 0) {
	    perror(dev);
	}
	printf("Read value = %d\n", read_value);
    }
    
    kbpause(delay);
    printf("Calling read (buf) on fd1...\n");
    if ( read(fd1, buf, count) < 0) {
	perror(dev);
    }
    printf("Read value (buf)= %d\n", *(int *)buf);

    kbpause(delay);
    printf("Calling read (buf) on fd2...\n");
    if ( read(fd2, buf, count) < 0) {
	perror(dev);
    }
    printf("Read value (buf)= %d\n", *(int *)buf);


    kbpause(delay);
    printf("Seeking SEEK_SET 0L on fd2...\n");
    if ( (ret = lseek(fd2, 0L, SEEK_SET)) < 0) {
	perror(dev);
    }
    printf("lseek() returned %d\n", ret);

    my_value[0] = 5;
    my_value[1] = 6;
    my_value[2] = 7;
    my_value[3] = 8;
    
    kbpause(delay);
    printf("Calling write on fd2...\n");
    if ( (ret = write(fd2, &my_value, sizeof(my_value))) < 0) {
	perror(dev);
    }
    printf("write() returned %d\n", ret);
    

    kbpause(delay);
    printf("Seeking SEEK_SET 0L on fd1...\n");
    if ( (ret = lseek(fd1, 0L, SEEK_SET)) < 0) {
	perror(dev);
    }
    printf("lseek() returned %d\n", ret);

    for(int i=0; i< 4; i++)
    {
	kbpause(delay);
	printf("Calling read on fd1...\n");
	if ( read(fd1, &read_value, count) < 0) {
	    perror(dev);
	}
	printf("Read value = %d\n", read_value);
    }




    kbpause(delay);
    kbpause(delay);
    printf("Closing fd1...\n");
    close(fd1);

    kbpause(delay);
    printf("Closing fd2...\n");
    close(fd2);


    return 0;
}
