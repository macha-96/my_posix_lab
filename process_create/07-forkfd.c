#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // 第一个命令行参数是打开的文件路径，第二个参数是将要往文件内写入的字符串
    if(argc < 3) {
		perror("too fiew arguments"); exit(-1);
    }

    int fd, pid, status;
    int err;
    size_t str_size = strlen(argv[2]);
    char buffer[10];
    memset(buffer,10,0);

    // 打开文件(如果文件不存在就创建文件)
    if((fd = open(argv[1], O_RDWR | O_TRUNC | O_CREAT)) < 0) {
    	perror("[MAIN]open file fail!\n"); exit(fd);
    }
    if(write(fd, argv[2], str_size) == -1) {
		perror("[MAIN]write file fail!\n");
    }

    // fork一个子进程，并在子进程内读取父进程已经打开的文件描述符
    if((pid = fork()) < 0) {
		perror("[MAIN]sub process create fail!\n"); exit(pid);
    }else if(!pid) {
		// 子进程
		sleep(3);
		lseek(fd,SEEK_SET,0);
		read(fd,buffer,str_size / 3);
		write(STDOUT_FILENO,buffer,str_size / 3);
		// printf("[MAIN][%d] buffer: %s\n",__LINE__,buffer);
    }else {
		// 父进程
		// sleep(3);
		
		/* wait函数用于使进程阻塞，直到有一个子进程终止或者该进程接收到一个特定的信号 */
		int sub_pid = wait(&status);
		if(sub_pid < 0) {
			// 如果wait出错返回-1，没有出错返回子进程的id
			perror("[MAIN]wait sub process fail!\n"); exit(sub_pid);
		}
		// status 用于存放进程结束状态

		lseek(fd,SEEK_CUR,1);
		read(fd,buffer,str_size / 3);
		write(STDOUT_FILENO,buffer,str_size / 3);
		write(STDOUT_FILENO,"\n",1);
		// printf("[MAIN][%d] buffer: %s\n",__LINE__,buffer);
    }
    exit(0);
}
