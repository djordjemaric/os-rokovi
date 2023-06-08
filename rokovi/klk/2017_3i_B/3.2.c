#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#define checkError(cond, msg) \
    do                        \
    {                         \
        if (!(cond))          \
        {                     \
            perror(msg);      \
            return 1;         \
        }                     \
    } while (0)

#define KB (10)
#define MB (20)
#define GB (30)

int main(int argc, char **argv)
{
    checkError(argc == 3, "args");

    struct stat fileInfo;
    checkError(-1 != stat(argv[1], &fileInfo), "stat");

    char *imeFajla;
    imeFajla = strrchr(argv[1], '/');
    int l = strlen(argv[1]);
    if (imeFajla == NULL || imeFajla == argv[l - 1])
        imeFajla = argv[1];
    else
        imeFajla++;
    int shift;
    char jedinica[3];
    if (!strcmp(argv[2], "KB"))
    {
        shift = KB;
        strcpy(jedinica, "KB");
    }
    else if (!strcmp(argv[2], "MB"))
    {
        shift = MB;
        strcpy(jedinica, "MB");
    }
    else if (!strcmp(argv[2], "GB"))
    {
        shift = GB;
        strcpy(jedinica, "GB");
    }
    int stepen = 1 << shift;
    double velicina = (double)fileInfo.st_size;
    velicina = ceil(velicina / (double)stepen);
    int konvertovanaVelicina = (int)velicina;

    printf("%s %d%s", imeFajla, konvertovanaVelicina, jedinica);
    return 0;
}