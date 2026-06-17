#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// 自定义环境变量数组，最后一个元素必须为NULL
const char *env_initial[] = {"USER=Unknown","PATH=/tmp",NULL};

int main(int argc, char *argv[]) {
    pid_t pid;

    // ====================== 第一次fork + execle ======================
    // fork() 创建子进程，失败返回-1，子进程返回0，父进程返回子进程PID
    if((pid = fork()) < 0) {
        perror("fork new child process error!\n");
    }else if(!pid) {  // 子进程分支
        printf("child process pid:%d\n",pid);

        /************************* execle 函数说明 *************************
         * 函数原型：int execle(const char *path, const char *arg, ..., char * const envp[]);
         * 1. 功能：执行**指定绝对路径**的可执行文件，完全替换子进程的代码和数据
         * 2. 特点：
         *    - l：list，参数以列表形式逐个传递，最后以NULL结尾
         *    - e：environment，**自定义环境变量**，覆盖进程默认环境变量
         *    - 必须使用可执行文件的【绝对路径】，不会去系统PATH中查找
         * 3. 参数：
         *    第1个参数：可执行文件绝对路径
         *    中间参数：传递给可执行文件的命令行参数（argv[0],argv[1]...）
         *    倒数第2个参数：(char*)0 等价于 NULL，标记参数列表结束
         *    最后1个参数：自定义环境变量数组
         ******************************************************************/
        if(execle("/home/ky/workspace/cprojects/posix_lab/process_create/echoall",                  "my_arg1","MY_ARG2",(char*)0,env_initial) < 0) {
            perror("execle error!\n");
        }
        // execle 成功后，后续代码不会执行；只有执行失败才会走到这里
        sleep(1);
    }

    // 父进程等待子进程结束，防止子进程变成僵尸进程
    if(waitpid(pid,NULL,0) < 0) {
        perror("wait child process terminated fail!\n");
    }
    write(STDOUT_FILENO,"\n\n",3);

    // ====================== 第二次fork + execlp ======================
    if((pid = fork()) < 0) {
        perror("fork new child process error!\n");
    }else if(!pid) {  // 子进程分支
        printf("child process pid:%d\n",pid);

        /************************* execlp 函数说明 *************************
         * 函数原型：int execlp(const char *file, const char *arg, ..., NULL);
         * 1. 功能：执行可执行文件，优先在**系统环境变量PATH**中查找文件
         * 2. 特点：
         *    - l：list，参数以列表形式逐个传递，最后以NULL结尾
         *    - p：path，**使用系统默认环境变量PATH**搜索可执行文件
         *    - 可以使用【相对路径/文件名】，无需写绝对路径
         * 3. 与execle的核心区别：
         *    - execlp：用系统环境变量，不能自定义环境变量
         *    - execle：用自定义环境变量，必须写绝对路径
         ******************************************************************/
        if(execlp("./echoall","my_arg_1","MY_ARG_2",NULL) < 0)
            perror("execlp error!\n");
        // execlp 成功后，后续代码不会执行
        sleep(1);
    }

    // 等待任意一个子进程结束
    if(wait(NULL) < 0)
        perror("wait child process terminated fail!\n");

    exit(0);
}
