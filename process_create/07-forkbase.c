#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int glob = 6;	    /* 全局变量，存放在全局静态区(Initialized Data)里面 */
char buf[] = "a write to stdout\n";

int main() {
    int var = 8;    /* 函数内局部变量，存放在栈空间内 */
    pid_t pid;
    if(write(STDOUT_FILENO,buf,sizeof(buf) - 1) != sizeof(buf) - 1) {
        perror("write error!");
    }
    printf("before fork\n");	/* we don't flush stdout */
    // printf("before fork");	/* we don't flush stdout */
    if((pid = fork()) < 0) {
        perror("fork error!");
    }else if(pid == 0) {
        // 这个部分是子进程执行的程序，fork在子进程内返回0
        glob++; var++;
    }else {
        // 这个部分是父进程执行的程序，fork在父进程内返回子进程的pid
        sleep(2);
    }
    // 打印进程id,父进程id,全局变量glob,局部变量var
    printf("pid: %d, ppid: %d, glob: %d, var: %d\n",getpid(), getppid(), glob, var);
    exit(0);
}
