#if !defined(PROCESS_H)
#define PROCESS_H

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct
{
    int id;
    double arrival_time;
    int burst_time;
    double remaining_time;
    double start_time;
    double completion_time;
    double turnaround_time;
    double waiting_time;
    void *queue;
} Thread;

extern unsigned int NUM_THREADS;
extern Thread *thread;

void thread_init(Thread *thread, unsigned int NUM_THREADS);
void thread_reset(Thread *thread);

#endif // PROCESS_H