#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>   // 共享内存核心头文件
#include <unistd.h>    // 系统调用，如read、sleep
#include <stdio.h>     // 标准输入输出
#include <stdlib.h>    // 退出函数、类型转换
#include <string.h>    // 字符串操作、内存初始化
#include <signal.h>    // 信号处理

// 定义共享内存的大小为4096字节
#define SHM_BUF_SIZE 4096

// 信号处理函数声明
static void myTerminate(int signo);
// 全局变量：共享内存ID，初始化为-1（无效值）
static int shm_id = -1;
// 全局变量：指向共享内存的指针，初始化为NULL
static char *shm_buf = NULL;

int main(int argc, char *argv[]) {
    // 参数校验：如果没有传入命令行参数，打印错误提示
    if(argc < 2) 
        perror("too few arguments!");

    // 共享内存键值：有参数则用参数转整数，无参数默认使用721
    int shm_key = argc < 2 ? 721 : atoi(argv[1]);

    // 遍历注册1~64号所有信号的处理函数，程序收到任意信号都会执行myTerminate
    for(int i = 1; i <= 64; ++i) {
        if(signal(i, myTerminate) < 0)
            perror("redefine my terminate callback function fail!");
    }

    // ===================== 1. 创建/获取共享内存 =====================
    // shmget：根据key创建共享内存
    // 0666：共享内存权限（所有用户可读可写）
    // IPC_CREAT：如果共享内存不存在则创建，存在则直接获取
    // 返回值：共享内存ID
    if((shm_id = shmget(shm_key, SHM_BUF_SIZE, 0666 | IPC_CREAT)) < 0) {
        perror("alloc share memory fail!");
        exit(EXIT_FAILURE);
    }

    // ===================== 2. 映射共享内存到进程虚拟地址空间 =====================
    // shmat：将共享内存挂载到当前进程的地址空间
    // NULL：由系统自动选择映射的虚拟地址
    // 0：默认读写权限
    // 返回值：指向共享内存的虚拟地址指针
    if((shm_buf = shmat(shm_id, NULL, 0)) < (char*)0) {
        perror("mapping share memory fail!");
        exit(EXIT_FAILURE);
    }

    // ===================== 3. 根据宏定义，执行读/写逻辑 =====================
#ifdef ENWR
    // 定义了ENWR宏：写入模式 -> 从标准输入读取数据，写入共享内存
    int len = 0;
    // 循环读取终端输入，写入共享内存
    while((len = read(STDIN_FILENO, shm_buf, SHM_BUF_SIZE - 1)) > 0);
    // 读取失败则打印错误
    if(len < 0) 
        perror("read from stdin fail!");
#else
    // 未定义ENWR宏：读取模式 -> 循环从共享内存读取数据并打印
    while(1) {
        // 阻塞等待：共享内存为空时一直循环等待
        while(!strlen(shm_buf));
        // 打印共享内存中的内容
        fputs(shm_buf, stdout);
        // 清空共享内存，准备接收下一次数据
        memset(shm_buf, 0, SHM_BUF_SIZE);
        // 休眠1秒，降低循环频率
        sleep(1);
    }
#endif

    // ===================== 4. 解除共享内存映射 =====================
    // shmdt：将当前进程与共享内存分离（仅断开连接，不删除物理内存）
    if(shmdt(shm_buf) < 0) 
        perror("detach share memory fail!");

    // ===================== 5. 删除系统中的共享内存 =====================
    // shmctl + IPC_RMID：真正释放共享内存的物理资源
    if(shmctl(shm_id, IPC_RMID, 0) < 0) 
        perror("delete share memory fail!");

    exit(EXIT_SUCCESS);
}

// 信号处理函数：程序被中断（Ctrl+C、kill等）时执行，用于安全释放共享内存
static void myTerminate(int signo) {
    printf("terminated!\n");
    // 无论收到什么信号，统一执行资源释放
    switch(signo) {
    case SIGTERM:  // 终止信号
        // 分离共享内存
        if(shmdt(shm_buf) < 0)
            perror("detach share memory fail!");
        // 删除共享内存
        if(shmctl(shm_id, IPC_RMID, 0) < 0)
            perror("delete share memory fail!");
        break;
    default:  // 其他所有信号
        if(shmdt(shm_buf) < 0)
            perror("detach share memory fail!");
        if(shmctl(shm_id, IPC_RMID, 0) < 0)
            perror("delete share memory fail!");
        break;
    }
    // 安全退出程序
    exit(EXIT_SUCCESS);
}
