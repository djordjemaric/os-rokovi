#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
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
    check_error(argc == 2, "args");

    int fd = open(argv[1], O_RDWR, 0644);
    check_error(fd != -1, "open");
    FILE *f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 7;
    lock.l_len = 5;
    check_error(fcntl(fd, F_SETLK, &lock) != -1, "error");

    lock.l_start = 26;
    lock.l_len = 5;
    check_error(fcntl(fd, F_SETLK, &lock) != -1, "error");

    lock.l_whence = SEEK_END;
    lock.l_start = -5;
    lock.l_len = 4;
    check_error(fcntl(fd, F_SETLK, &lock) != -1, "error");

    while (1)
    {
        char rec[10];
        scanf("%s", rec);
        if (!strcmp(rec, "quit"))
            break;
    }
    lock.l_whence = SEEK_SET;
    lock.l_type = F_UNLCK;
    lock.l_len = 0;
    lock.l_start = 0;
    check_error(fcntl(fd, F_SETLK, &lock) != -1, "error");
    exit(EXIT_SUCCESS);
}