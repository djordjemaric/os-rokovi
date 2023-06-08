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

#define MAX_LEN (256)

int main(int argc, char **argv)
{
    check_error(argc == 3, "args");
    struct stat file_info;
    check_error(stat(argv[1], &file_info) != -1, "stat");
    char trazena_rec[MAX_LEN];
    strcpy(trazena_rec, argv[2]);
    char uneta_rec[MAX_LEN];
    int fd = open(argv[1], O_RDWR, 0777);
    check_error(fd != -1, "open");
    FILE *f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    while (fscanf(f, "%s", uneta_rec) != EOF)
    {
        if (strcmp(uneta_rec, trazena_rec) == 0)
        {
            struct flock lock;
            lock.l_whence = SEEK_SET;
            lock.l_type = F_WRLCK;
            lock.l_start = ftell(f);
            lock.l_len = -strlen(trazena_rec);

            check_error(fcntl(fd, F_GETLK, &lock) != -1, "fcntl");

            if (lock.l_type != F_UNLCK)
            {
                char tip = 'r';
                if (lock.l_type == F_WRLCK)
                    tip = 'w';
                printf("%d %c\n", ftell(f) - strlen(trazena_rec), tip);
            }
        }
    }
}