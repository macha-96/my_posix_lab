/*
 * 这份代码意在测试子进程终止的时候，父进程内的wait函数的获取子进程退出状态的功能
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void pr_exit(int);

int main() {
    pid_t pid; int status;
    if((pid = fork()) < 0) {
        perror("create sub process 1 fail!"); abort();
    }else if(!pid) {
        sleep(1);
        printf("in child process\n");
        // exit 正常退出子进程
        exit(114);	// 子进程正常退出，返回值为114
    }
    printf("in parent process\n");
    if(wait(&status) < 0) {
        perror("wait sub process fail");
    }
    pr_exit(status);

    if((pid = fork()) < 0) {
        perror("create sub process 2 fail!"); abort();
    }else if(!pid) {
        sleep(1);
        // abort() 会导致子进程异常退出，并且释放一个异常退出的信号
        abort();    // 子进程异常退出
    }
    if(wait(&status) < 0) {
        perror("wait sub process fail");
    }    
    pr_exit(status);
    

    if((pid = fork()) < 0) {
        perror("create sub process 3 fail!"); abort();
    }else if(!pid) {
        sleep(1);
        int a = 0;
        int sum = 10 / a;    // 触发除0异常，会导致系统杀死进程，同时产生一个退出信号
    }
    if(wait(&status) < 0) {
        perror("wait sub process fail");
    }    
    pr_exit(status);
    
    exit(0);
}

void pr_exit(int status) {
    if(WIFEXITED(status))
	printf("normally termination, low order 8 bit of exit status: %d\n",WEXITSTATUS(status));
    else if(WIFSIGNALED(status))
	printf("abnormally termination, signal number: %d\n",WTERMSIG(status));
}

