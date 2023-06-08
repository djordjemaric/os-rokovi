#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
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

int jesteBroj(char *s, int d)
{
    for (int i = 0; i < d; i++)
    {
        if (!isdigit(s[i]))
        {
            if (i == 0 && s[i] == '-')
                continue;
            return 0;
        }
    }
    return 1;
}

#define MAX_LEN 257
int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    struct stat file_info;
    check_error(stat(argv[1], &file_info) != -1, "stat");

    int fd = open(argv[1], O_RDONLY, 0777);
    check_error(fd != -1, "open");
    FILE *file = fdopen(fd, "r");
    check_error(file != NULL, "fdopen");
    int brojac = 0;
    char rec[MAX_LEN];

    while (fscanf(file, "%s", rec) != EOF)
    {
        int d = strlen(rec);
        if (jesteBroj(rec, d))
        {
            struct flock lock;
            lock.l_whence = SEEK_SET;
            lock.l_type = F_RDLCK;
            lock.l_start = ftell(file);
            lock.l_len = -d;
            errno = 0;
            if (fcntl(fd, F_SETLK, &lock) == -1)
            {
                if (errno == EACCES || errno == EAGAIN)
                    continue;
                else
                    check_error(false, "fcntl");
            }
            brojac++;
        }
    }
    printf("%d", brojac);
}