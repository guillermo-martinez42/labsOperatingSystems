#if !defined(SCHEDULER_H)
#define SCHEDULER_H

#include <time.h>
#include "../process/process.h"
#include "queue.h"
#include "../logs/logger.h"

extern int quantum;

void *fifo_thread(void *arg);
void fifo_schedule(Thread *thread);
void *rr_thread(void *arg);
void rr_schedule(Thread *thread, int *scheduled);
void *sjf_thread(void *arg);
void sjf_schedule(Thread *thread);
void *srtf_thread(void *arg);
void srtf_schedule(Thread *thread);

#endif // SCHEDULER_H