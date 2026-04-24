#include "scheduler.h"
#include "fifo.c"
#include "rr.c"
#include "sjf.c"
#include "srtf.c"

void run_fifo(Thread *threads)
{
    printf("--- FIFO Scheduling --- \n");
    pthread_t p_threads[NUM_THREADS];
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL) return;

    init_queue(q);

    for (int i = 0; i < (int)NUM_THREADS; i++)
    {
        threads[i].queue = q;
        if (pthread_create(&p_threads[i], NULL, fifo_thread, &threads[i]) != 0)
        {
            perror("pthread_create");
            return;
        }
    }

    int scheduled = 0;
    while (scheduled < (int)NUM_THREADS)
    {
        Thread *curr_thread = dequeue(q);
        if (curr_thread != NULL)
        {
            fifo_schedule(curr_thread);
            scheduled++;
        }
    }

    pthread_mutex_destroy(&q->lock);
    free(q);
    print_stats(threads, NUM_THREADS);
}

void run_rr(Thread *threads)
{
    printf("--- Round Robin Scheduling (Quantum 2) --- \n");
    thread_reset(threads);

    pthread_t p_threads[NUM_THREADS];
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL) return;

    init_queue(q);

    for (int i = 0; i < (int)NUM_THREADS; i++)
    {
        threads[i].queue = q;
        if (pthread_create(&p_threads[i], NULL, rr_thread, &threads[i]) != 0)
        {
            perror("pthread_create");
            return;
        }
    }

    int scheduled = 0;
    while (scheduled < (int)NUM_THREADS)
    {
        Thread *curr_thread = dequeue(q);
        if (curr_thread != NULL)
        {
            rr_schedule(curr_thread, &scheduled);
            scheduled++;
        }
    }

    pthread_mutex_destroy(&q->lock);
    free(q);
    print_stats(threads, NUM_THREADS);
}

void run_sjf(Thread *threads)
{
    printf("--- SJF Scheduling--- \n");
    thread_reset(threads);

    pthread_t p_threads[NUM_THREADS];
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL) return;

    init_queue(q);

    for (int i = 0; i < (int)NUM_THREADS; i++)
    {
        threads[i].queue = q;
        if (pthread_create(&p_threads[i], NULL, sjf_thread, &threads[i]) != 0)
        {
            perror("pthread_create");
            return;
        }
    }

    int scheduled = 0;
    while (scheduled < (int)NUM_THREADS)
    {
        Thread *curr_thread = dequeue(q);
        if (curr_thread != NULL)
        {
            sjf_schedule(curr_thread);
            scheduled++;
        }
    }

    pthread_mutex_destroy(&q->lock);
    free(q);
    print_stats(threads, NUM_THREADS);
}

void run_srtf(Thread *threads)
{
    printf("--- SRTF Scheduling--- \n");
    thread_reset(threads);

    pthread_t p_threads[NUM_THREADS];
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL) return;

    init_queue(q);

    for (int i = 0; i < (int)NUM_THREADS; i++)
    {
        threads[i].queue = q;
        if (pthread_create(&p_threads[i], NULL, srtf_thread, &threads[i]) != 0)
        {
            perror("pthread_create");
            return;
        }
    }

    int scheduled = 0;
    while (scheduled < (int)NUM_THREADS)
    {
        Thread *curr_thread = get_SRTF(q);
        if (curr_thread != NULL)
        {
            srtf_schedule(curr_thread, &scheduled);
            scheduled++;
        }
    }

    pthread_mutex_destroy(&q->lock);
    free(q);
    print_stats(threads, NUM_THREADS);
}