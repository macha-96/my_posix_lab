#include "MyThreadPool.h"
#include <stdlib.h>
#include <stdio.h>

static void *mainLoop(void *arg) {
    my_thread_pool_t *self = (my_thread_pool_t *)arg;	    // 将参数的类型强制转换为my_thread_pool_t类型，以便解析结构体成员
    while (true) {
        pthread_mutex_lock(&self->taskq_mtx);		    // 线程在操作任务队列之前需要先获取锁
        while (!self->task_queue_head && !self->shutdown)   // 等待条件变量的条件：任务队列为空，且线程池处在运行状态（终止标志为false）
            pthread_cond_wait(&self->taskq_cond, &self->taskq_mtx);

        if (self->shutdown && !self->task_queue_head) {	    // 如果线程已经终止（终止标志为true）且任务队列为空，结束线程
            pthread_mutex_unlock(&self->taskq_mtx);
            break;
        }

	// 从任务队列尾部（链表尾节点）中取出一个任务，并从任务队列（链表）中删除
        task_queue_node_t* tail = self->task_queue_head->prior;
        if (tail == self->task_queue_head) {
            self->task_queue_head = NULL;
        }else {
            tail->prior->next = self->task_queue_head;
            self->task_queue_head->prior = tail->prior;
        }
        self->task_queue_size--;
        pthread_mutex_unlock(&self->taskq_mtx);		    // 操作任务队列结束，释放锁

        tail->task(tail->args);				    // 执行任务队列中取出的任务
        free(tail->args);				    // 执行完成后才释放取出的任务节点
        free(tail);
    }
    return NULL;
}

my_thread_pool_t *createThreadPool(size_t thread_num) {
    int err;

    // 创建线程池对象
    my_thread_pool_t *obj = (my_thread_pool_t *)malloc(sizeof(my_thread_pool_t));
    if (obj == NULL) {
        perror("create thread pool fail");
        return NULL;
    }

    // 初始化线程池对象的成员
    obj->task_queue_head = NULL;    // 任务队列的队头指针（任务队列是一个双循环链表）
    obj->task_queue_size = 0;	    // 任务队列的大小
    obj->thread_num = thread_num;   // 线程数
    obj->shutdown = false;	    // 线程池终止标志

    // 创建锁和条件变量（在创建线程之前创建好，保证不发生竞争访问）
    if ((err = pthread_mutex_init(&obj->taskq_mtx, NULL))) {
        printf("initial mutex fail, error number is %d\n", err);
        free(obj);
        return NULL;
    }
    if ((err = pthread_cond_init(&obj->taskq_cond, NULL))) {
        printf("initial condition variable fail, error number is %d\n", err);
        pthread_mutex_destroy(&obj->taskq_mtx);
        free(obj);
        return NULL;
    }

    // 创建线程id列表
    obj->thread_list = (pthread_t *)malloc(thread_num * sizeof(pthread_t));
    if (obj->thread_list == NULL) {
	// 创建线程列表失败，进行回滚处理，销毁之前已经分配好的对象内存，锁和条件变量
        perror("create thread list fail");
        pthread_cond_destroy(&obj->taskq_cond);
        pthread_mutex_destroy(&obj->taskq_mtx);
        free(obj);
        return NULL;
    }

    // 初始化所有的线程
    for (size_t i = 0; i < thread_num; ++i) {
        if ((err = pthread_create(obj->thread_list + i, NULL, mainLoop, obj)) != 0) {
            printf("create thread fail, error number is %d\n", err);
        }
    }

    return obj;
}

int enqueueTask(my_thread_pool_t* self, size_t priority, void *(*task)(void*), void* args) {
    pthread_mutex_lock(&self->taskq_mtx);	// 操作队列任务队列之前首先获取锁

    // 创建任务队列的节点
    task_queue_node_t *node = (task_queue_node_t *)malloc(sizeof(task_queue_node_t));
    if (!node) {
        perror("add task list node fail");
        pthread_mutex_unlock(&self->taskq_mtx);
        return -1;
    }

    // 初始化任务队列节点的值
    node->task = task;
    node->args = args;
    node->priority = priority;

    // 将任务节点插入到任务队列链表中
    if (!self->task_queue_head) {		// 任务队列为空的分支
        node->next = node;
        node->prior = node;
        self->task_queue_head = node;
    }else {					// 任务队列不为空的分支
        node->next = self->task_queue_head;
        node->prior = self->task_queue_head->prior;
        self->task_queue_head->prior->next = node;
        self->task_queue_head->prior = node;
        self->task_queue_head = node;
    }

    self->task_queue_size++;			// 任务队列大小 + 1
    pthread_cond_signal(&self->taskq_cond);	// 通知条件变量改变状态(对条件变量的操作必须在获取锁的情况下)
    pthread_mutex_unlock(&self->taskq_mtx);	// 操作队列完成以后释放锁
    return 0;
}

int deleteThreadPool(my_thread_pool_t* self) {
    pthread_mutex_lock(&self->taskq_mtx);	// 操作shutdown终止标志位之前先获取锁
    self->shutdown = true;			// 置终止标志位为true，通知所有的线程结束运行
    // 这样做的好处是可以让线程执行完当前的任务，在正常返回，不会从执行任务过程中突然结束，避免潜在的内存泄露问题
    pthread_cond_broadcast(&self->taskq_cond);	// 通知所有条件变量改变状态，不要等待了，执行shutdown
    pthread_mutex_unlock(&self->taskq_mtx);	// 操作完shutdown标志位之后，释放锁

    // 阻塞主线程，等待所有线程结束
    for (size_t i = 0; i < self->thread_num; ++i) {
        pthread_join(self->thread_list[i], NULL);   // 阻塞主线程，等待所有线程结束
    }

    // 释放锁和条件变量
    pthread_mutex_destroy(&self->taskq_mtx);
    pthread_cond_destroy(&self->taskq_cond);

    // 删除任务列表（链表），一个一个析构全部节点以及其分配的参数内存
    while (self->task_queue_head) {
        task_queue_node_t* tail = self->task_queue_head->prior;
        if (tail == self->task_queue_head) {
            self->task_queue_head = NULL;
        } else {
            tail->prior->next = self->task_queue_head;
            self->task_queue_head->prior = tail->prior;
        }
        free(tail->args);
        free(tail);
    }

    // 析构线程列表和线程池对象
    free(self->thread_list);
    free(self);
    return 0;
}
