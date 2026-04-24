#include "process.h"

unsigned int NUM_THREADS = 0;

void thread_init(Thread *thread, unsigned int NUM_THREADS)
{
    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread[i].id = i;
        unsigned int seed = (unsigned int)time(NULL) ^ ((unsigned int)thread[i].id * 2654435761u);

        thread[i].arrival_time = rand_r(&seed) % 11;    // 0-10 seconds
        thread[i].burst_time = (rand_r(&seed) % 10) + 1; // 1-10 seconds
        thread[i].remaining_time = thread[i].burst_time;
        thread[i].start_time = 0;
        thread[i].completion_time = 0;
        thread[i].turnaround_time = 0;
        thread[i].waiting_time = 0;
    }
}

void thread_reset(Thread *thread) {
    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread[i].remaining_time = thread[i].burst_time;
        thread[i].start_time = 0;
        thread[i].completion_time = 0;
        thread[i].turnaround_time = 0;
        thread[i].waiting_time = 0;
    }
}