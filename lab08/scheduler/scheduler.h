#if !defined(SCHEDULER_H)
#define SCHEDULER_H

#include <time.h>
#include "../process/process.h"
#include "queue.h"
#include "../logs/logger.h"

// FIFO
void *fifo_thread(void *arg);
void fifo_schedule(Thread *thread);

// Round Robin
extern int quantum;

void *rr_thread(void *arg);
void rr_schedule(Thread *thread, int *scheduled);

// SJF
void *sjf_thread(void *arg);
void sjf_schedule(Thread *thread);

// SRTF Preemption Configuration
#define TICK 0.01
#define GET_RUN_TIME(remaining) ((remaining) < TICK ? (remaining) : TICK)

void *srtf_thread(void *arg);
void srtf_schedule(Thread *thread, int *scheduled);

#endif // SCHEDULER_H