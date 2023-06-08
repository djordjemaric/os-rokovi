#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#define checkError(cond,msg)\
	do{\
		if(!(cond)){\
		perror(msg);\
		exit(EXIT_FAILURE);\
		}\
	}while(0)

#define MAX_SIZE 1024

int openFile(char *path,char mode)
{
    int flags = 0;
    switch(mode)
    {
        case 'r':
            flags = O_RDONLY;
            break;
        case 'w':
            flags = O_CREAT | O_WRONLY | O_TRUNC;
            break;
        case 'a':
            flags = O_CREAT | O_WRONLY | O_APPEND; 
    }
    int fd;
    checkError((fd = open(path,flags)) != -1, "open");
    return fd;
}

void writeToFile(int fdSrc, int fdDest)
{
    int bytesRead;
    char memBuff[MAX_SIZE];
    while((bytesRead = read(fdSrc,memBuff,MAX_SIZE))>0)
        checkError(write(fdDest,memBuff,bytesRead) != -1, "write");
}

int main(int argc, char **argv)
{
    checkError(argc == 3, "argv");
    int fd = openFile(argv[2],argv[1][1]);
    switch(argv[1][1])
    {
        case 'r':
            writeToFile(fd,STDOUT_FILENO);
            break;
        case 'w':
            writeToFile(STDIN_FILENO,fd);
            break;
        case 'a':
            writeToFile(STDIN_FILENO,fd);
        default:
            break;
    }
    close(fd);
    return 0;
}