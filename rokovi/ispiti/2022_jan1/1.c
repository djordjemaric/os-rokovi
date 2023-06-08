#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#define BUFF_SIZE (1024)

#define check_error(cond, msg)                                                                       \
    do                                                                                               \
    {                                                                                                \
        if (!(cond))                                                                                 \
        {                                                                                            \
            perror(msg);                                                                             \
            fprintf(stderr, "File: %s\nFunction: %s\nLine number:%d", __FILE__, __func__, __LINE__); \
            exit(EXIT_FAILURE);                                                                      \
        }                                                                                            \
    } while (0)

int main(int argc, char **argv)
{
    check_error(argc == 3, "args");

    int fd_src = open(argv[1], O_RDONLY, 0644);
    check_error(fd_src != -1, "open");

    int fd_dest = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    check_error(fd_dest != -1, "open");

    int bytesRead = 0;
    char buff[BUFF_SIZE];
    while ((bytesRead = read(fd_src, buff, BUFF_SIZE)) > 0)
    {
        check_error(write(fd_dest, buff, bytesRead) != -1, "write");
    }
    check_error(unlink(argv[1]) != -1, "unlink");
    close(fd_src);
    close(fd_dest);
    exit(EXIT_SUCCESS);
}