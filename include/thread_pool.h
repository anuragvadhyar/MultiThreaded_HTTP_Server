
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>

#define THREAD_NUM 4


typedef struct node TreeNode;

typedef struct Task {
    int client_fd;
    TreeNode *root;
    int (*task_func)(int, TreeNode *);
} Task;

extern int task_count;
extern pthread_mutex_t mutexQueue;
extern pthread_cond_t condQueue;
extern Task taskQueue[100];

void submitTask(Task t);
void execute_task(Task *t);
void* start_thread(void *args);

#endif // THREAD_POOL_H

