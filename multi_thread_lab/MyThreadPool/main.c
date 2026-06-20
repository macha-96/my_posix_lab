#include "MyThreadPool.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct taskargs {
    int count;
    char str[1024];
}taskargs_t;

static void* testTask(void* arg) {
    taskargs_t* task_args = (taskargs_t*)arg;
    for(size_t i = 0; i < 5; ++i) {
        printf("%s: %d\n",task_args->str, task_args->count);
        usleep(200000);
    }
    return NULL;
}

int main(void) {
    my_thread_pool_t* thread_pool_p = createThreadPool(4);
    if (!thread_pool_p) {
        fprintf(stderr, "create thread pool fail\n");
        exit(EXIT_FAILURE);
    }

    for(size_t i = 0; i < 16; ++i) {
        taskargs_t* test_args = (taskargs_t*)malloc(sizeof(taskargs_t));
        test_args->count = i;
        // test_args->str = "Task_";
        sprintf(test_args->str, "Task_%ld", i);
	enqueueTask(thread_pool_p, 0, testTask, test_args);
    }

    if (deleteThreadPool(thread_pool_p) < 0) {
        perror("delete Thread Pool fail");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
