/*
 * 在Linux系统下使用命名管道实现进程间通信
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>

#define BUF_SIZE 1025

#ifdef ENWR
#include <time.h>
#endif

int main() {
    int fifofd, len;
    char buf[BUF_SIZE] = {0};
    mode_t mod = 0666;
    
    // 创建命名管道文件，如果管道存在，就会直接报错但是不影响程序运行
    if(mkfifo("/tmp/myfifo",mod) < 0) perror("create fifo fail!\n");

// 使用命令：gcc -DENWR 08-simple_fifo.c -o wrfifo，编译为写端
#ifdef ENWR
    time_t timestamp;	// 构造时间戳

    printf("write fifo\n");

    // 打开命名管道，此时是发送方，采用写入模式
    if((fifofd = open("/tmp/myfifo",O_WRONLY)) < 0)
	perror("open fifo fail!\n");

    while((len = read(STDIN_FILENO, buf, BUF_SIZE - 1)) > 0) {
        char tx_buffer[BUF_SIZE * 2] = {0};
        time(&timestamp); // 获取系统时间
        
        // 构造发送的buffer（进程id，发送时间(用ctime将发送时间转换为字符串格式)，buf为用户输入数据）
        sprintf(tx_buffer,"[transmit pid: %d][time: %s]: %s",getpid(),ctime(&timestamp),buf);
        
        // 写命名管道，发送数据
        if(write(fifofd, tx_buffer, strlen(tx_buffer)) < 0)	
            perror("write to fifo fail!\n");
    }
    if(len < 0) perror("read data from stdio fail!\n");

// 使用命令：gcc 08-simple_fifo.c -o rdfifo，正常编译为读端
#else
    printf("read fifo\n");

    // 打开命名管道，由于是接收方，采用只读模式
    if((fifofd = open("/tmp/myfifo",O_RDONLY)) < 0) 
	perror("open fifo fail!\n");

    // 从命名管道中读数据
    while((len = read(fifofd, buf, BUF_SIZE - 1)) > 0) {
        printf("receive data from fifo: %s\n",buf);
    }

    if(len < 0) perror("read data from fifo fail!\n");
#endif
    close(fifofd);
    return 0;
}

/* 注：
 * 在只编译读端：gcc 08-simple_fifo.c -o rdfifo
 * 的情况下，可以通过向/tmp/myfifo管道直接写入数据，例如：
 * ```bash
 * echo "Ciallo～(∠・ω< )⌒ ★" > /tmp/myfifo
 * ```
 * 从而向读端的进程发送消息，本质为echo进程与rdfifo进程交换数据
 */

