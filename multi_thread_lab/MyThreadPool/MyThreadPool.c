#include "MyThreadPool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void mainLoop(my_thread_pool_t *self) {
    while(true) {
		pthread_mutex_lock(&self->taskq_mtx);// 操作任务队列之前要先获取锁
		while(!self->task_queue_head) 
			pthread_cond_wait(&self->taskq_cond, &self->taskq_mtx);
		task_queue_node_t* tail = self->task_queue_head->prior;
		// 从任务队列中取出任务节点
		tail->prior->next = self->task_queue_head;
		self->task_queue_head->prior = tail->prior;
		self->task_queue_size--;
		pthread_mutex_unlock(&self->taskq_mtx);// 操作任务队列之后释放锁
		tail->task(tail->args);// 执行任务队列中取出的任务
		free(tail);
		if(tail == self->task_queue_head) 
			self->task_queue_head = NULL;
    }
}

my_thread_pool_t *createThreadPool(size_t thread_num) {
    int err;
    my_thread_pool_t *obj = NULL;
    if((obj = (my_thread_pool_t *)malloc(thread_num * sizeof(my_thread_pool_t))) == NULL) {
		perror("create thread pool fail");
		return NULL;
    }

    obj->task_queue_head = NULL;
    obj->task_queue_size = 0;
    
    // create thread_num threads
    obj->thread_num = thread_num;
    if((obj->thread_list = (pthread_t*)malloc(sizeof(pthread_t))) == NULL) {
		perror("create thread list fail");
		return NULL;
    } 
    for(size_t i = 0; i < thread_num; ++i) {
		if((err = pthread_create(obj->thread_list + i,NULL,mainLoop,obj)) != 0) {
			printf("create thread fail, error number is %d\n",err);
			continue;
		}
    }
    
    // initial the mutex and condition_variable
    if(err = pthread_mutex_init(&obj->taskq_mtx,NULL)) 
	printf("initial mutex fail, error number is %d\n",err);
    if(err = pthread_cond_init(&obj->taskq_cond,NULL))
	printf("initial condition variable fail, error number is %d\n",err);
    return obj;
}

int enqueueTask(my_thread_pool_t* self, size_t priority, void *(*task)(void*), void* args) {
    pthread_mutex_lock(&self->taskq_mtx);// 操作任务队列之前要先获取锁
    if(!self->task_queue_head) {
		self->task_queue_head = (task_queue_node_t*)malloc(sizeof(task_queue_node_t));
		if(!self->task_queue_head) {
			perror("add task list node fail");
			return -1;
		}
		self->task_queue_head->next = self->task_queue_head;
		self->task_queue_head->prior = self->task_queue_head;
    }else {
		task_queue_node_t *ptr = (task_queue_node_t*)malloc(sizeof(task_queue_node_t));
		if(!self->task_queue_head) {
			perror("add task list node fail");
			return -1;
		}
		ptr->next = self->task_queue_head;
		ptr->prior = self->task_queue_head->prior;
		self->task_queue_head->prior->next = ptr;
		self->task_queue_head->prior = ptr;
		self->task_queue_head = ptr;
    }
    self->task_queue_head->task = task;
    // self->task_queue_head->args = args;
    size_t args_size = strlen((char*)args);
    self->task_queue_head->args = malloc(args_size);
    strncpy((char*)self->task_queue_head->args, (char*)args, args_size);

    self->task_queue_head->priority = priority;
    self->task_queue_size++;
    pthread_cond_signal(&self->taskq_cond);// 向条件变量发送变为真信号
    pthread_mutex_unlock(&self->taskq_mtx);// 操作任务队列结束以后要释放锁
    return 0;
}

int deleteThreadPool(my_thread_pool_t* self) {
    int err;
    if(err = pthread_mutex_destroy(&self->taskq_mtx)) return err;
    if(err = pthread_cond_destroy(&self->taskq_cond)) return err;
    self->task_queue_head->prior->next = NULL;
    for(task_queue_node_t* ptr = self->task_queue_head; ptr != NULL; ) {
		task_queue_node_t* tmp = ptr->next;
		free(ptr->args);
		free(ptr);
		ptr = tmp;
    }
    for(size_t i = 0; i < self->thread_num; ++i) {
		if(pthread_cancel(self->thread_list[i]))
			printf("cancel thread %ld fail\n",self->thread_list[i]);
    }
    free(self->thread_list);
    free(self);
    return 0;
}
