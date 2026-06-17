#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    struct stat buf;
    if(argc != 2) {
        perror("Usage: stat <pathname>\n");
        exit(-1);
    }
    
    // stat函数还有类似的两个函数fstat和lstat
    if(lstat(argv[1], &buf) != 0) {
        // if(stat(argv[1], &buf) != 0) {
            perror("stat error.\n");
        exit(-1);
    }

    /* 注意：
     * fstat和stat的功能是完全一样的，区别就是fstat的第一个参数是一个文件描述符，而stat的第一个参数是文件的路径字符串(const char *)
     * lstat和stat的区别：这两个函数的区别主要在于对于软链接，用stat会得到软链接所指向的文件的参数而不是软链接这个文件本身的参数，而使用lstat的话会得到软链接这个文件本身的参数
     */

    printf("#link:\t\t\t\t%ld\n",buf.st_nlink);
    printf("#UID:\t\t\t\t%d\n",buf.st_uid);
    printf("#Size:\t\t\t\t%ld\n",buf.st_size);
    printf("#Block Size:\t\t\t%ld\n",buf.st_blksize);
    printf("#block:\t\t\t\t%ld\n",buf.st_blocks);
    exit(0);
}
