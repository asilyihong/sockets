#include <time.h>
#include <sys/time.h>

#include "utils.h"

void record_time_gettimeofday()
{
    typedef struct timeval mTimeval;
    static long long prev_time  = -1;
    
    mTimeval time;
    gettimeofday(&time, NULL);

    long long curr_time = time.tv_sec * 1000000 + time.tv_usec;
    if (-1 != prev_time) {
	// has previous record
	fprintf(stderr, "time: %lld\tdiff: %lld\n", curr_time, curr_time - prev_time);
    } else {
	// no previous record
	fprintf(stderr, "time: %lld\n", curr_time);
    }

    prev_time = curr_time;
}

void record_time_clock() {
    static long prev_time = -1;
    clock_t time = clock();
    float ll_time = (float)time;
    if (-1 != prev_time) {
	// has previous record
	fprintf(stderr, "time: %f\tdiff: %ld\n", ll_time, time - prev_time);
    } else {
	// no previous record
	fprintf(stderr, "time: %f\n", ll_time);
    }

    prev_time = (long) time;
}

void record_time()
{
    record_time_gettimeofday();
}


