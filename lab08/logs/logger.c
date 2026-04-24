#include "logger.h"

void print_dataset(Thread *thread, int count)
{
    printf("Dataset: %d threads, ", count);

    printf("Burst Times: [");
    for (int i = 0; i < count; i++)
    {
        printf("%d%s", thread[i].burst_time, (i == count - 1) ? "" : ", ");
    }
    printf("], ");

    printf("Arrival Times: [");
    for (int i = 0; i < count; i++)
    {
        printf("%.0f%s", thread[i].arrival_time, (i == count - 1) ? "" : ", ");
    }
    printf("]\n\n");
}

void print_timestamp()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[30];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Formats to: [Fri Mar 21 15:00:00 2025]
    strftime(buffer, 30, "[%a %b %d %H:%M:%S %Y]", timeinfo);
    printf("%s ", buffer);
}

void log_arrival(Thread *thread)
{
    if (thread->arrival_time == 0)
    {
        print_timestamp();
        printf("Process %d (Burst %d): Arrived\n", thread->id, thread->burst_time);
    }
}

void log_start(Thread *thread, double arrival_time, double waiting_time)
{
    print_timestamp();
    if (arrival_time > 0)
    {
        printf("Process %d (Burst %d): Arrived at %.0f, Started (waited %.2f seconds)\n",
               thread->id, thread->burst_time, arrival_time, waiting_time);
    }
    else
    {
        if (waiting_time > 0)
        {
            printf("Process %d (Burst %d): Started (waited %.2f seconds)\n",
                   thread->id, thread->burst_time, waiting_time);
        }
        else
        {
            printf("Process %d (Burst %d): Started\n", thread->id, thread->burst_time);
        }
    }
}

void log_preemption(Thread *thread)
{
    print_timestamp();
    printf("Process %d (Burst %d remaining): Preempted\n", thread->id, (int)thread->remaining_time);
}

void log_completion(Thread *thread)
{
    print_timestamp();
    printf("Process %d (Burst %d): Completed\n", thread->id, thread->burst_time);
}

void print_stats(Thread *thread, unsigned int count)
{
    printf("\nWaiting Times: [");
    double total_waiting = 0;
    for (unsigned int i = 0; i < count; i++)
    {
        printf("%.2f", thread[i].waiting_time);
        if (i < count - 1)
            printf(", ");
        total_waiting += thread[i].waiting_time;
    }
    printf("] \n");
    
    printf("Avg Waiting Time: %.2f seconds \n", total_waiting / count);
    
    printf("Turnaround Times: [");
    double total_turnaround = 0;
    for (unsigned int i = 0; i < count; i++)
    {
        printf("%.2f", thread[i].turnaround_time);
        if (i < count - 1)
            printf(", ");
        total_turnaround += thread[i].turnaround_time;
    }
    printf("] \n");
    
    printf("Avg Turnaround Time: %.2f seconds \n\n", total_turnaround / count);
}