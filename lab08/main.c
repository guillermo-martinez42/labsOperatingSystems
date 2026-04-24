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

    run_fifo(thread);
    run_rr(thread);
    run_sjf(thread);
    run_srtf(thread);

    return EXIT_SUCCESS;
}