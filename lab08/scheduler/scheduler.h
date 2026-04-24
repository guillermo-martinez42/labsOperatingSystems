#if !defined(SCHEDULER_H)
#define SCHEDULER_H

#include <time.h>
#include "../process/process.h"
#include "queue.h"
#include "../logs/logger.h"

// SRTF Preemption Configuration
#define TICK 0.01
#define GET_RUN_TIME(remaining) ((remaining) < TICK ? (remaining) : TICK)

// Scheduler Interfaces
void run_fifo(Thread *threads);
void run_rr(Thread *threads);
void run_sjf(Thread *threads);
void run_srtf(Thread *threads);

// Internal Scheduler Functions
void *fifo_thread(void *arg);
void fifo_schedule(Thread *thread);

extern int quantum;
void *rr_thread(void *arg);
void rr_schedule(Thread *thread, int *scheduled);

void *sjf_thread(void *arg);
void sjf_schedule(Thread *thread);

void *srtf_thread(void *arg);
void srtf_schedule(Thread *thread, int *scheduled);

#endif // SCHEDULER_H