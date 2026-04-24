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
    
    NUM_THREADS = rand_r(&seed) % 10 + 5;     // 5-15 Threads

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

    free(fifo);

    print_stats(thread, NUM_THREADS);

    /*

    // Round Robin

    pthread_t threads_rr[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads_rr[i], NULL, fifo_schedule, thread[i]) != 0)
        {
            perror("pthread_create");
            free(threads_rr);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_join(threads_rr[i], NULL) != 0)
        {
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }

    free(threads_rr);

    // SJF

    pthread_t threads_sjf[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads_sjf[i], NULL, fifo_schedule, thread[i]) != 0)
        {
            perror("pthread_create");
            free(threads_sjf);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_join(threads_sjf[i], NULL) != 0)
        {
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }

    free(threads_sjf);

    // SRTF

    pthread_t threads_srtf[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads_srtf[i], NULL, fifo_schedule, thread[i]) != 0)
        {
            perror("pthread_create");
            free(threads_srtf);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_join(threads_srtf[i], NULL) != 0)
        {
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }

    free(threads_srtf);

    */

    return EXIT_SUCCESS;
}