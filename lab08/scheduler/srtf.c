#include "scheduler.h"

static double base_time_srtf = 0;

void *srtf_thread(void *arg)
{
    Thread *thread = (Thread *)arg;
    sleep((unsigned int)thread->arrival_time);
    insert_SJF(thread->queue, thread);
    log_arrival(thread);
    return NULL;
}

void srtf_schedule(Thread *thread, int *scheduled)
{
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    thread->start_time = start.tv_sec + start.tv_nsec / 1e9;

    if (base_time_srtf == 0)
    {
        base_time_srtf = thread->start_time - thread->arrival_time;
    }

    double waiting_time = thread->start_time - (base_time_srtf + thread->arrival_time) - (thread->burst_time - thread->remaining_time);
    log_start(thread, thread->arrival_time, waiting_time);

    while (thread->remaining_time > 0)
    {
        double run_time = GET_RUN_TIME(thread->remaining_time);
        usleep((unsigned int)(run_time * 1e6));
        thread->remaining_time -= run_time;

        if (thread->remaining_time <= 0)
        {
            thread->remaining_time = 0;
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            thread->completion_time = now.tv_sec + now.tv_nsec / 1e9;
            thread->turnaround_time = thread->completion_time - (base_time_srtf + thread->arrival_time);
            thread->waiting_time = thread->turnaround_time - thread->burst_time;
            log_completion(thread);
            return;
        }

        Queue *q = (Queue *)thread->queue;
        pthread_mutex_lock(&q->lock);
        if (q->front != NULL && q->front->thread->remaining_time < thread->remaining_time)
        {
            pthread_mutex_unlock(&q->lock);
            (*scheduled)--;
            log_preemption(thread);
            insert_SJF(q, thread);
            return;
        }
        pthread_mutex_unlock(&q->lock);
    }
}