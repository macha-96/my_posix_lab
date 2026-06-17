#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    char helloworld[] = "Hello world!\n";
    write(STDOUT_FILENO,helloworld,sizeof(helloworld));
    return 0;
}
