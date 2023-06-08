#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#define check_error(cond, msg)                                                                           \
    do                                                                                                   \
    {                                                                                                    \
        if (!(cond))                                                                                     \
        {                                                                                                \
            perror(msg);                                                                                 \
            fprintf(stderr, "File: %s\n Function: %s \n, Line number:%d", __FILE__, __func__, __LINE__); \
            exit(EXIT_FAILURE);                                                                          \
        }                                                                                                \
    } while (0)

// usage ./a.out path a b
int main(int argc, char **argv)
{
    check_error(argc == 4, "args");
    int a, b;
    a = atoi(argv[2]);
    b = atoi(argv[3]);
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = (off_t)a;
    lock.l_len = (off_t)b;
    int fd;
    check_error(fd = (open(argv[1], O_RDWR)), "open");

    check_error(fcntl(fd, F_GETLK, &lock) != -1, "fcntl");

    switch (lock.l_type)
    {
    case F_RDLCK:
        printf("shared lock\n");
        break;
    case F_WRLCK:
        printf("exclusive lock\n");
        break;
    case F_UNLCK:
        printf("unlocked\n");
        break;
    }
    close(fd);
    exit(EXIT_SUCCESS);
}
