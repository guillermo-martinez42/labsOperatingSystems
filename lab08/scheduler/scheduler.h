#include <time.h>
#include "../process/process.h"
#include "queue.h"
#include "../logs/logger.h"

void *fifo_thread(void *arg);
void fifo_schedule(Thread *thread);
void *round_robin_schedule(Thread *thread);
void *sjf_schedule(Thread *thread);
void *srtf_schedule(Thread *thread);