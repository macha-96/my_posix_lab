#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void sigUsr(int); /* one handler for both signal */
int main() {
    for(int i = 1; i < 64; ++i) {
        if(signal(i,sigUsr) == SIG_ERR) 
            printf("can not catch signal: %d.\n",i);
    }
    if(signal(SIGTSTP,SIG_DFL) == SIG_ERR)
	printf("can not catch signal SIGTSTP.\n");

    if(signal(SIGUSR1,SIG_IGN) == SIG_ERR) 
	perror("can not catch SIGUSR1\n");
    if(signal(SIGUSR2,sigUsr) == SIG_ERR)
	perror("can not catch SIGUSR2\n");
    if(signal(SIGINT,SIG_IGN) == SIG_ERR)
	perror("cannot catch SIGINT\n");
    
    while(1) {
        printf("abc\n");
        pause();
    }
    return 0;
}

static void sigUsr(int signo) {
    switch(signo) {
    case SIGUSR1:
        printf("received signal SIGUSR1\n");
        break;
    case SIGUSR2:
        printf("reveived signal SIGUSR2\n");
        break;
    default:
        printf("received signal number: %d\n",signo);
        break;
    }
}
