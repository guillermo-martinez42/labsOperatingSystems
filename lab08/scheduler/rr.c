#include "scheduler.h"

static double base_time_rr = 0;
int quantum = 2;

void *rr_thread(void *arg)
{
    Thread *thread = (Thread *)arg;
    sleep((unsigned int)thread->arrival_time);
    enqueue(thread->queue, thread);
    log_arrival(thread);
    return NULL;
}

void rr_schedule(Thread *thread, int *scheduled)
{
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    thread->start_time = start.tv_sec + start.tv_nsec / 1e9;

    if (base_time_rr == 0)
    {
        base_time_rr = thread->start_time - thread->arrival_time;
    }

    double waiting_time = thread->start_time - (base_time_rr + thread->arrival_time) - (thread->burst_time - thread->remaining_time);
    log_start(thread, thread->arrival_time, waiting_time);

    if (thread->remaining_time - quantum <= 0)
    {
        sleep((unsigned int)thread->remaining_time);
        thread->remaining_time = 0;

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        thread->completion_time = now.tv_sec + now.tv_nsec / 1e9;
        thread->turnaround_time = thread->completion_time - (base_time_rr + thread->arrival_time);
        thread->waiting_time = thread->turnaround_time - thread->burst_time;

        log_completion(thread);
    }
    else
    {
        sleep(quantum);
        thread->remaining_time -= quantum;
        (*scheduled)--;

        log_preemption(thread);
        enqueue(thread->queue, thread);
    }
}