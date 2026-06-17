/*
 * 三个线程并发打印hello world!
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

static void *task_0(void *vargp);
static void *task_1(void *vargp);
static void *task_2(void *vargp);

int main() {
    pthread_t tid_0, tid_1, tid_2;
    int err;
    if((err = pthread_create(&tid_0,NULL,task_0,NULL)) != 0) {
	perror("create thread error");
	printf("err number: %d\n",err);
    }
    if((err = pthread_create(&tid_1,NULL,task_1,NULL)) != 0) {
	perror("create thread error");
	printf("err number: %d\n",err);
    }
    if((err = pthread_create(&tid_2,NULL,task_2,NULL)) != 0) {
	perror("create thread error");
	printf("err number: %d\n",err);
    }
    
    if(pthread_join(tid_0,NULL) < 0) perror("thread task_0 join error");
    if(pthread_join(tid_1,NULL) < 0) perror("thread task_1 join error");
    if(pthread_join(tid_2,NULL) < 0) perror("thread task_2 join error");
    
    exit(EXIT_SUCCESS);
}

static void *task_0(void *vargp) {
    for(size_t i = 0; i < 100; ++i) {
    	printf("[%s]: hello world!\n",__FUNCTION__);
	// sleep(1);
	usleep(5000);
    }
    return NULL;
}

static void *task_1(void *vargp) {
    for(size_t i = 0; i < 100; ++i) {
    	printf("[%s]: hello world!\n",__FUNCTION__);
	// sleep(1);
	usleep(5000);
    }
    return NULL;
}

static void *task_2(void *vargp) {
    for(size_t i = 0; i < 100; ++i) {
    	printf("[%s]: hello world!\n",__FUNCTION__);
	// sleep(1);
	usleep(5000);
    }
    return NULL;
}
