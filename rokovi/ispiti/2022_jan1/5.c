#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
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
    check_error(argc == 2, "args");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");
    FILE *f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    char rec[MAX_LEN];
    int izmenjenih = 0;
    while (fscanf(f, "%s", rec) == 1)
    {
        char *error = NULL;

        int broj = strtol(rec, &error, 10);

        // jeste broj
        if (rec[0] != '\0' && *error == '\0')
        {
            int n = strlen(rec);
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = ftell(f);
            lock.l_len = -n;

            errno = 0;
            int ret_val = fcntl(fd, F_SETLK, &lock);

            if (ret_val != -1)
            { // uspesno smo zakljucali
                for (int i = 0; rec[i] != 0; i++)
                    rec[i] = '#';
                fseek(f, -n, SEEK_CUR);
                izmenjenih += n;
                fprintf(f, "%s", rec);
                lock.l_type = F_UNLCK;
                check_error(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");
            }
            else
            {
                if (errno != EACCES && errno != EAGAIN)
                {
                    check_error(0, "fail");
                }

                continue;
            }
        }
    }

    printf("%d\n", izmenjenih);
    fclose(f);
    exit(EXIT_SUCCESS);
}