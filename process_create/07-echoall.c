#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {
    extern char** environ;	// 获取系统环境变量
    
    for(int i = 0; i < argc; ++i) // 打印所有的命令行参数
	printf("argv[%d]: %s\n",i,argv[i]);
    
    write(STDOUT_FILENO,"\nenvironment variables:\n",strlen("\nenvironment variables:\n"));
    
    for(char** ptr = environ; *ptr != 0; ++ptr) // 打印环境变量
	printf("%s\n",*ptr);
    
    /*
    printf("hostname is : %s\n",getenv("HOSTNAME"));
    setenv("HOSTNAME","Anon",0);
    printf("hostname is : %s\n",getenv("HOSTNAME"));
    setenv("HOSTNAME","Anon",1);
    printf("hostname is : %s\n",getenv("HOSTNAME"));
    */
    exit(0);
}
