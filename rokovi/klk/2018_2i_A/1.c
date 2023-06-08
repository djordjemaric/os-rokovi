#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <utime.h>

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
    checkError(argc == 3, "args");

    unsigned long vreme = atol(argv[2]);
    struct stat fileInfo;
    checkError(stat(argv[1], &fileInfo) != -1, "stat");

    struct utimbuf novoVreme;
    novoVreme.actime = (time_t)vreme;
    novoVreme.modtime = (time_t)vreme;

    checkError(utime(argv[1], &novoVreme) != -1, "utime");
    exit(EXIT_SUCCESS);
}