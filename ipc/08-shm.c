#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SHM_MAX_SIZE 32768

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
}my_semcfg;

static void myTerminate(int signo);

int semid, shmid;
key_t sem_key, shm_key;
char *shm_buf = NULL;
char buf[SHM_MAX_SIZE];
struct sembuf sem_op_arr[2] = {
    {0,1,IPC_NOWAIT},{0,-1,0}
};

int main(int argc, char *argv[]) {
    size_t len = 0;
    
    memset(buf,0,SHM_MAX_SIZE);

    // create share memory (physic memory)
    shm_key = argc > 1 ? (key_t)atoi(argv[1]) : (key_t)721;
    if((shmid = shmget(shm_key, SHM_MAX_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("create share memory error");
        exit(EXIT_FAILURE);
    }
    printf("create share memory ,id:%d\n",shmid);

    // attach share memory
    if((shm_buf = shmat(shmid, NULL, 0)) < (char *)0) {
        perror("attach share memory error");
        exit(EXIT_FAILURE);
    }
    printf("attach share memory ,address:%p\n",(void *)shm_buf);
    
    // create semaphone set
    sem_key = argc > 2 ? (key_t)atoi(argv[2]) : (key_t)114;
    if((semid = semget(sem_key, 1, IPC_CREAT | 0666)) < 0) {
        perror("create semaphone set error");
        exit(EXIT_FAILURE);
    }
    printf("create semaphone set ,id:%d\n",semid);

    // initialized semaphone
    my_semcfg.val = 1;
    if(semctl(semid,0,SETVAL,my_semcfg) == -1) {
        perror("set semaphone initial value error");
        exit(EXIT_FAILURE);
    }
    printf("initialized semaphone, semaphone val:%d\n",semctl(semid,0,GETVAL));

    for(size_t i = 1; i <= 64; ++i) {
        if(signal(i,myTerminate) < 0) perror("reset signal error");
    }

#ifdef ENWR
    while((len = read(STDIN_FILENO, buf, SHM_MAX_SIZE - 1)) > 0) {
        // wait semaphone here
        if(semop(semid,sem_op_arr + 1,1) < 0) {
            perror("wait semaphone error");
            exit(EXIT_FAILURE);
        }

        memcpy(shm_buf, buf, (SHM_MAX_SIZE - 1) * sizeof(char));
        
        // release semaphone here
        if(semop(semid,sem_op_arr,1) < 0) {
            perror("wait semaphone error");
            exit(EXIT_FAILURE);
        }

        memset(buf,0,SHM_MAX_SIZE * sizeof(char));
    }
    if(len < 0) perror("error occured when reading data from stdio!");
#else
    bool flag = false;
    while(1) {
        // wait semaphone here
        if(semop(semid,sem_op_arr + 1,1) < 0) {
            perror("wait semaphone error");
            exit(EXIT_FAILURE);
        }
        
        if(strlen(shm_buf)) {
            memcpy(buf, shm_buf, (SHM_MAX_SIZE - 1) * sizeof(char));
            memset(shm_buf, 0, SHM_MAX_SIZE);
            flag = true;
        }

        // release semaphone here
        if(semop(semid,sem_op_arr,1) < 0) {
            perror("release semaphone error");
            exit(EXIT_FAILURE);
        }
        
        if(flag) {
            printf("receive data: %s\n",buf);
            memset(buf,0,SHM_MAX_SIZE * sizeof(char));
            flag = false;
        }
    }
#endif
    
    // remove semaphone and share memory
    if(semctl(semid,0,IPC_RMID) < 0) 
	perror("remove semaphone set error");
    if(shmdt(shm_buf) < 0) perror("detach share memory error");
    if(shmctl(shmid,IPC_RMID,NULL) < 0) 
	perror("remove share memory error");
    exit(EXIT_SUCCESS);
}

static void myTerminate(int signo) {
    printf("receive signal: %d\n", signo);
    
    // remove semaphone and share memory
    if(semctl(semid,0,IPC_RMID) < 0) 
	perror("remove semaphone set error");
    if(shmdt(shm_buf) < 0) perror("detach share memory error");
    if(shmctl(shmid,IPC_RMID,NULL) < 0) 
	perror("remove share memory error");
    exit(EXIT_SUCCESS);
}

