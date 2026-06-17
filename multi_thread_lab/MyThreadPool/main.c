#include "MyThreadPool.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct taskargs {
    int count;
    char *str;
}taskargs_t;

static void* testTask(taskargs_t* task_args) {
    for(size_t i = 0; i < 10; ++i) {
	printf("%s: %d\n",task_args->str, task_args->count);
	usleep(200000);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // write(STDOUT_FILENO,"hello world!\n",sizeof("hello world!\n"));
    my_thread_pool_t* thread_pool_p = createThreadPool(4);

    for(size_t i = 0; i < 16; ++i) {
	taskargs_t test_args;
	test_args.count = i;
	test_args.str = "Task_";
	enqueueTask(thread_pool_p, 0, testTask, &test_args);
    }

    for(size_t i = 0; i < thread_pool_p->thread_num; ++i) {
	int err = pthread_join(thread_pool_p->thread_list[i],NULL);
	if(err) 
	    printf("thread %ld join fail, err number is %d\n",thread_pool_p->thread_list[i],err);
    }

    if(deleteThreadPool(thread_pool_p) < 0) {
	perror("delete Thread Pool fail");
	exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
