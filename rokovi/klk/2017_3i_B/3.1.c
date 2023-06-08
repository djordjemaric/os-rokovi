#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#define DAY_IN_SEC (24 * 60 * 60)
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
    checkError(argc == 2, "args");
    struct stat fileInfo;
    checkError(-1 != stat(argv[1], &fileInfo), "stat");
    time_t sekundeOdPristupa = fileInfo.st_atime;
    time_t sekundeOdModifikacije = fileInfo.st_mtime;
    time_t razlika = sekundeOdPristupa - sekundeOdModifikacije;
    int rezultat = razlika / DAY_IN_SEC;
    printf("%d", rezultat);
}