#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "process/process.h"
#include "scheduler/scheduler.h"
#include "logs/logger.h"

int main(void)
{
    unsigned int seed = (unsigned int)time(NULL) ^ (2654435761u);

    NUM_THREADS = rand_r(&seed) % 11 + 5; // 5-15 Threads

    Thread thread[NUM_THREADS];

    thread_init(thread, NUM_THREADS);
    print_dataset(thread, NUM_THREADS);

    // FIFO

    printf("--- FIFO Scheduling --- \n");

    pthread_t threads_fifo[NUM_THREADS];
    Queue *fifo = (Queue *)malloc(sizeof(Queue));

    if (fifo == NULL)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    init_queue(fifo);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread[i].queue = fifo;
        if (pthread_create(&threads_fifo[i], NULL, fifo_thread, &thread[i]) != 0)
        {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    // Dequeue and schedule threads as they arrive
    int scheduled = 0;
    while (scheduled < NUM_THREADS)
    {
        Thread *curr_thread = dequeue(fifo);
        if (curr_thread != NULL)
        {
            fifo_schedule(curr_thread);
            scheduled++;
        }
    }

    pthread_mutex_destroy(&fifo->lock);
    free(fifo);

    print_stats(thread, NUM_THREADS);

    // Round Robin

    printf("--- Round Robin Scheduling (Quantum 2) --- \n");

    thread_reset(thread);

    pthread_t threads_rr[NUM_THREADS];
    Queue *rr = (Queue *)malloc(sizeof(Queue));

    if (rr == NULL)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    init_queue(rr);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread[i].queue = rr;
        if (pthread_create(&threads_rr[i], NULL, rr_thread, &thread[i]) != 0)
        {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    // Schedule threads as they arrive and re-enqueue if not finished
    scheduled = 0;
    while (scheduled < NUM_THREADS)
    {
        Thread *curr_thread = dequeue(rr);
        if (curr_thread != NULL)
        {
            rr_schedule(curr_thread, &scheduled);
            scheduled++;
        }
    }

    pthread_mutex_destroy(&rr->lock);
    free(rr);

    print_stats(thread, NUM_THREADS);

    // SJF

    printf("--- SJF Scheduling--- \n");

    thread_reset(thread);

    pthread_t threads_sjf[NUM_THREADS];
    Queue *sjf = (Queue *)malloc(sizeof(Queue));

    if (sjf == NULL)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    init_queue(sjf);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread[i].queue = sjf;
        if (pthread_create(&threads_sjf[i], NULL, sjf_thread, &thread[i]) != 0)
        {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    // Schedule threads as SJF
    scheduled = 0;
    while (scheduled < NUM_THREADS)
    {
        Thread *curr_thread = dequeue(sjf);
        if (curr_thread != NULL)
        {
            sjf_schedule(curr_thread);
            scheduled++;
        }
    }

    pthread_mutex_destroy(&sjf->lock);
    free(sjf);

    print_stats(thread, NUM_THREADS);

    // SRTF

    printf("--- SRTF Scheduling--- \n");

    thread_reset(thread);

    pthread_t threads_srtf[NUM_THREADS];
    Queue *srtf = (Queue *)malloc(sizeof(Queue));

    if (srtf == NULL)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    init_queue(srtf);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread[i].queue = srtf;
        if (pthread_create(&threads_srtf[i], NULL, srtf_thread, &thread[i]) != 0)
        {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    // Schedule threads as SRTF
    scheduled = 0;
    while (scheduled < NUM_THREADS)
    {
        Thread *curr_thread = get_SRTF(srtf);
        if (curr_thread != NULL)
        {
            srtf_schedule(curr_thread, &scheduled);
            scheduled++;
        }
    }

    pthread_mutex_destroy(&srtf->lock);
    free(srtf);

    print_stats(thread, NUM_THREADS);

    return EXIT_SUCCESS;
}