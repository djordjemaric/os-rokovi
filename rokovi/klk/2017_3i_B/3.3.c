#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define checkError(cond, msg)   \
    do                          \
    {                           \
        if (!(cond))            \
        {                       \
            perror(msg);        \
            exit(EXIT_FAILURE); \
        }                       \
    } while (0)

void makeDir(char *path, char *mode)
{
    mode_t prava = strtol(mode, NULL, 8);
    mode_t oldUmask = umask(0);
    checkError(-1 != mkdir(path, prava), "mkdir");
    umask(oldUmask);
}

void makeFile(char *path, char *mode)
{
    mode_t prava = strtol(mode, NULL, 8);
    mode_t oldUmask = umask(0);
    checkError(-1 != open(path, O_CREAT | O_RDONLY, prava), "creat");
    umask(oldUmask);
}

int main(int argc, char **argv)
{
    checkError(argc == 4, "args");
    struct stat fileInfo;
    checkError(stat(argv[2], &fileInfo) == -1, "fajl postoji");

    switch (argv[1][1])
    {
    case 'f':
        makeFile(argv[2], argv[3]);
        break;
    case 'd':
        makeDir(argv[2], argv[3]);
        break;
    default:
        break;
    }
}