#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

int main() {
    int fd = -1;	// 定义文件描述符
    char buf[100];	// 读取文件的缓冲区数组
    memset(buf,0,100);	// 填充数组的所有数值为0

    // 打开文件
    if((fd = open("/tmp/test.txt",O_RDONLY)) < 0) {
        perror("open"); exit(-1);
    }

    read(fd,buf,3);	// 从文件中读取三个字节

    write(STDOUT_FILENO,buf,3);	// 向标准输出的文件描述符中写入三个字节，从而打印缓冲区的内容
    
    lseek(fd,1,SEEK_CUR);// 移动文件指针，从当前位置向后移动1个字节
    read(fd,buf,3);	// 继续读取3个字节
    write(STDOUT_FILENO,buf,3);// 打印读取的三个字节
    close(fd);		// 关闭文件，操作系统将会释放分配给文件的内存资源
    return 0;
}
