#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

// 全局参数数组，最多存20个参数
char *cmdArgv[20] = {0};

// 函数声明
int parseArgs(char *cmd_line);

int main(int argc, char *argv[]) {
    pid_t pid;
    char buf[100];
    int ret_val;

    printf("MyShellV2# ");
    fflush(stdout);

    while(1) {
        // 读取用户输入
        fgets(buf, 100, stdin);

        // 处理空输入
        if (buf[0] == '\n') {
            printf("MyShellV2# ");
            fflush(stdout);
            continue;
        }

        // 去掉末尾的换行符
        buf[strcspn(buf, "\n")] = '\0';

        // 创建子进程
        if((pid = fork()) < 0) {
            perror("fork child process fail!\n");
            abort();
        }else if(!pid) {
            // 子进程：解析参数 + 执行命令
            parseArgs(buf);
            execvp(cmdArgv[0], cmdArgv);
            // 如果execvp失败，打印错误并退出
            perror("execvp failed");
            exit(1);
        }

        // 父进程等待子进程结束
        wait(&ret_val);
        printf("MyShellV2# ");
        fflush(stdout);
    }
    exit(0);
}

// 核心：解析命令行，拆分命令和参数，存入cmdArgv
int parseArgs(char *cmd_line) {
    int i = 0;
    char *p = cmd_line;

    // 清空上一次的参数数组
    memset(cmdArgv, 0, sizeof(cmdArgv));

    // 跳过开头的空格、制表符
    while (*p != '\0' && isspace((unsigned char)*p)) {
        p++;
    }

    // 按空格分割字符串
    while (*p != '\0' && i < 19) {
        // 当前位置就是一个参数的起始位置
        cmdArgv[i++] = p;

        // 找到参数的结尾（空格/结束符）
        while (*p != '\0' && !isspace((unsigned char)*p)) {
            p++;
        }

        // 如果不是结尾，把空格变成字符串结束符，然后跳过空格
        if (*p != '\0') {
            *p = '\0';
            p++;
            // 继续跳过连续空格
            while (*p != '\0' && isspace((unsigned char)*p)) {
                p++;
            }
        }
    }

    // 最后一个参数必须是NULL，execvp要求
    cmdArgv[i] = NULL;
    return 0;
}
