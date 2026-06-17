/*
 * 实验：使用管道(pipe)实现父进程和子进程之间进行数据交换
 * 子进程接收，父进程发送
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024
#define CHILD_NOTICE "recive from parent process: "

int main() {
    pid_t pid;
    int pipefd[2], n;
    char buf[BUF_SIZE] = {0};

    if(pipe(pipefd) < 0) perror("create pipe fail!\n");
    
    if((pid = fork()) < 0) perror("create child process fail!\n");
    else if(!pid) {
        // 子进程执行的代码
        close(pipefd[1]); // 关闭子进程的写管道描述符，保留读管道描述符
        // 子进程用read读管道内的数据，如果读到数据了就把数据打印出来，否则就会阻塞
        // 特殊情况，如果父进程终止了，管道的输入端将会全部都被关闭，这个时候read会返回0
        while((n = read(pipefd[0], buf, BUF_SIZE)) > 0) {
            write(STDOUT_FILENO, CHILD_NOTICE, sizeof(CHILD_NOTICE));
            write(STDOUT_FILENO, buf, n);
            write(STDOUT_FILENO, "\n",1);
        }
        if(n < 0) perror("read pipe error!\n");	// 子进程读管道错误
        close(pipefd[0]);   // 关闭所有管道描述符
        printf("exit child process.\n");
        exit(0); // 子进程正常退出
    }
    
    // 以下的代码均只有父进程会执行
    close(pipefd[0]);	// 关闭父进程的读管道（父进程只负责发送）
    
    // 读取标准输入，如果有读到键盘的标准输入的话就打印出来，没有读到的话将会阻塞
    while((n = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
	write(pipefd[1], buf, n); // 父进程向管道写数据，如果管道满了，将会阻塞
	// 如果调用write的这个时候管道的读端全部都被关闭，就会产生SIGPIPE信号，然后终止进程

    if(n < 0) perror("read stdin error!\n");
    
    close(pipefd[1]);
    wait(NULL);
    printf("exit parent process.\n");
    return 0;
}

