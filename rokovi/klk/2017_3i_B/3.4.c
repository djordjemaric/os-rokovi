#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define checkError(cond, msg)   \
    do                          \
    {                           \
        if (!(cond))            \
        {                       \
            perror(msg);        \
            exit(EXIT_FAILURE); \
        }                       \
    } while (0)

int main(int argc, char **argv)
{
    checkError(argc == 4, "args");
    int offset = atoi(argv[2]);
    int duzina = atoi(argv[3]);

    int fd;
    int flags = O_RDONLY;
    checkError(-1 != (fd = open(argv[1], flags)), "open");
    off_t l = lseek(fd, offset, SEEK_SET);

    char memBuff[duzina + 1];
    int readBytes;
    checkError(-1 != (readBytes = read(fd, memBuff, duzina)), "read");
    memBuff[readBytes] = '\0';
    checkError(write(STDOUT_FILENO, memBuff, readBytes) != -1, "write");
    exit(EXIT_SUCCESS);
}