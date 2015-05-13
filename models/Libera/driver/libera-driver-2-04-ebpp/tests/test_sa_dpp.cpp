#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h> // memcpy()

#include "libera.h"

#define HIST_SIZE 0x10000
#define HIST_TIME 30L

void help()
{
		
    printf("\n Usage : test_sa_sample -h -f <filename> \n switches: \n\t\t -h    this message \n\t\t -f <filename>  write to file <filename>,\n\t\t otherwise write to /mnt/nfs/histogram.dat\n");   

}


int main(int argc, char **argv)
{
    int ch = -1;
    int fd1;
    // struct timeval tv;
    unsigned int histogram[HIST_SIZE];
    int tmp;
    time_t curr_time, old_time;
    
    char *dev = "/dev/libera.sa";
    int enable_r = 1;
    //int ret;
    libera_atom_sa_t libera_sa_sample;
    FILE * out_file = NULL;
    int events = 0;

    char *out_file_name="/mnt/nfs/histogram.dat";

    // Here we handle command line swithches	

    while ( (ch = getopt( argc, argv, "f:h" )) != -1 ) {
        switch ( ch ) {
            case 'f': out_file_name=optarg; break;
            case 'h': help();return 0; break;
        }
    }

   

    	
    // Register as notofocation recipient
    //printf("Opening fd1->%s in O_RDONLY mode...\n", dev);
    if ((fd1 = open(dev, O_RDONLY)) < 0) {
	perror(dev);
        printf("Can't open device 'libera_sa'");
	return -1;
    }

    // initialization of histogram
    for (tmp = 0; tmp < HIST_SIZE; tmp ++)
	histogram[tmp] = 0;
    time(&old_time);

    out_file = fopen(out_file_name,"w");
    if (out_file) {
        for(tmp = 0; tmp < HIST_SIZE; tmp++)
            fprintf(out_file, "%d\t%ld\n",tmp,histogram[tmp]);
        fclose(out_file);
        printf("written to file @ %ld, %d new events \n",old_time, events);
	    fflush(stdout);
    }

	

    if (enable_r)
    {
	
	while (true)
	{
	    if ((read(fd1, &libera_sa_sample, 
		      sizeof(libera_atom_sa_t))) < 0) {
		perror(dev);
		return -1;
	    }
	    // Report the catch...
	    //gettimeofday(&tv, NULL);
	    //printf("Time: %lu%06lu  %d\n", tv.tv_sec, tv.tv_usec,
            //libera_sa_sample.sample);

	    //printf("%d\n", libera_sa_sample.sample);
	    //fflush(stdout);
	    histogram [libera_sa_sample.sample]++;
            events ++;
            time(&curr_time);
            if (((int)curr_time - (int)old_time) >= HIST_TIME) {
                old_time = curr_time;
                out_file = fopen(out_file_name,"w");
                if (out_file) {
                    for(tmp = 0; tmp < HIST_SIZE; tmp++)
                        fprintf(out_file, "%d\t%ld\n",tmp,histogram[tmp]);
                    fclose(out_file);
                    printf("written to file @ %ld, %d new events \n",curr_time, events);
                }
                events = 0;
            }
	}
    }
    
    //printf("Closing fd1...\n");
    close(fd1);


    return 0;
}
