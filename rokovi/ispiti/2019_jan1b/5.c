#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define MAX_LEN 257
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
    char rec[MAX_LEN];
    while (fscanf(f, "%s", rec) == 1)
    {
        int broj = atoi(rec);
        if (broj >= 1000 && broj <= 9999)
        {
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = ftell(f);
            lock.l_len = -4;
            errno = 0;
            if (fcntl(fd, F_SETLK, &lock) == -1)
            {
                if (errno == EACCES || errno == EAGAIN)
                    continue;
                else
                    check_error(0, "fcntl");
            }
            else
            {
                fseek(f, -4, SEEK_CUR);
                fprintf(f, "####");
            }
        }
    }
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    check_error(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");
    exit(EXIT_SUCCESS);
}