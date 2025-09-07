#include <stdio.h>
#include <pthread.h>
#include "thread_pool.h"

int task_count = 0;
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;
Task taskQueue[100];

void submitTask(Task t)
{
    printf("signal called here\n");
    pthread_mutex_lock(&mutexQueue);
    taskQueue[task_count++] = t;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void execute_task(Task *t)
{
    t->task_func(t->client_fd, t->root);
}

void* start_thread(void *args)
{
    int *int_ptr = (int*)args;
    int value = *int_ptr;
    while(1)
    {
        Task task;
        pthread_mutex_lock(&mutexQueue);
        printf("task count is %d for thread %d\n", task_count, value);
        while(task_count == 0)
        {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }
        printf("okay thread %d is taking a task out!\n", value);
        printf("%d got the mutex\n", value);
        task = taskQueue[0];
        for(int i = 0; i < task_count - 1; i++)
        {
            taskQueue[i] = taskQueue[i+1];
        }
        task_count--;
        printf("thread %d reduced the task count to %d\n", value, task_count);
        pthread_mutex_unlock(&mutexQueue);
        execute_task(&task);
    }
    return NULL;
}