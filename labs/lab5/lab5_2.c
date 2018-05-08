#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    int fd1,fd2;
    char s[16];
    fd1 = open("entrada.txt", O_RDONLY,0666);
    fd2 = open("saida.txt", O_WRONLY|O_CREAT,0666);
    close(0);
    dup(fd1);
    read(0,s,sizeof(s));
    close(1);
    dup(fd2);
    write(1,s,sizeof(s));
    return 0;
}