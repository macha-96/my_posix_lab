#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SHM_BUF_SIZE 4096

static void myTerminate(int signo);
static int shm_id = -1;
static char *shm_buf = NULL;

int main(int argc, char *argv[]) {
    if(argc < 2) perror("too few arguments!");

    int shm_key = argc < 2 ? 721 : atoi(argv[1]);
 
    for(int i = 1; i <= 64; ++i) {
	if(signal(i,myTerminate) < 0) 
	    perror("redefine my terminate callback function fail!");
    }

    // 创建共享内存（获取物理内存资源）
    if((shm_id = shmget(shm_key, SHM_BUF_SIZE, 0666 | IPC_CREAT)) < 0) {
        perror("alloc share memory fail!");
        exit(EXIT_FAILURE);
    }
    
    // 将共享内存（物理内存）映射到进程内的某个内存空间（虚拟内存空间）
    if((shm_buf = shmat(shm_id, NULL, 0)) < (char*)0) {
        perror("mapping share memory fail!");
        exit(EXIT_FAILURE);
    }

#ifdef ENWR
    int len = 0;
    while((len = read(STDIN_FILENO,shm_buf,SHM_BUF_SIZE - 1)) > 0);
    if(len < 0) perror("read from stdin fail!");
#else
    while(1) {
        while(!strlen(shm_buf));
        fputs(shm_buf,stdout);
        memset(shm_buf,0,SHM_BUF_SIZE);
        sleep(1);
    }
#endif

    // 解除虚拟内存空间到共享物理内存之间的映射
    if(shmdt(shm_buf) < 0) perror("detach share memory fail!");

    if(shmctl(shm_id,IPC_RMID,0) < 0) perror("delete share memory fail!");

    exit(EXIT_SUCCESS);
}

static void myTerminate(int signo) {
    printf("terminated!\n");
    switch(signo) {
    case SIGTERM:
        if(shmdt(shm_buf) < 0) 
            perror("detach share memory fail!");
        if(shmctl(shm_id,IPC_RMID,0) < 0) 
            perror("delete share memory fail!");
        break;
    default:
        if(shmdt(shm_buf) < 0) 
            perror("detach share memory fail!");
        if(shmctl(shm_id,IPC_RMID,0) < 0) 
            perror("delete share memory fail!");
        break;
    }
    exit(EXIT_SUCCESS);
}
