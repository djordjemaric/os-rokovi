#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
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
#define MAX_LEN 257
int main(int argc, char **argv)
{
    check_error(argc == 4, "args");

    int fd;
    check_error((fd = open(argv[1], O_RDWR, 0777)) != -1, "open");

    FILE *f;
    check_error((f = fdopen(fd, "r+")) != NULL, "fdopen");
    char ucitana_rec[MAX_LEN];
    int brojac = 0;
    while (fscanf(f, "%s", ucitana_rec) != EOF)
    {
        if (strcmp(argv[2], ucitana_rec) == 0)
        {
            struct flock lock;
            lock.l_whence = SEEK_SET;
            lock.l_type = F_WRLCK;
            lock.l_start = ftell(f);
            lock.l_len = -strlen(argv[2]);
            errno = 0;
            if (fcntl(fd, F_SETLK, &lock) == -1)
            {
                if (errno == EACCES || errno == EAGAIN)
                {
                    brojac++;
                    continue;
                }
                else
                    check_error(0, "fcntl");
            }
            else
            {
                check_error(fseek(f, -strlen(ucitana_rec), SEEK_CUR) != -1, "fseek");
                fprintf(f, "%s", argv[3]);
            }
        }
    }

    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    check_error(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");

    printf("%d", brojac);
    fclose(f);
    exit(EXIT_SUCCESS);
}