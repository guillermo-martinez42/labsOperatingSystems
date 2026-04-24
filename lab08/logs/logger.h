#include <time.h>
#include <stdio.h>
#include "../process/process.h"

void print_dataset(Thread *thread, int count);
void print_timestamp();
void log_arrival(Thread *thread);
void log_start(Thread *thread, double arrival_time, double waiting_time);
void log_preemption(Thread *thread);
void log_completion(Thread *thread);
void print_stats(Thread *thread, unsigned int count);