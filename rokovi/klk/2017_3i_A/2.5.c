#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define checkError(cond, msg)                    \
    do                                           \
    {                                            \
        if (!(cond))                             \
        {                                        \
            perror(msg);                         \
            printf("%s:%d", __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                  \
        }                                        \
    } while (0)
#define REG 0
#define DIR 1
#define CHR 2
#define BLK 3
#define LNK 4
#define FIF 5
#define SOC 6
#define UKU 7
int brojaci[8];
int obradi(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    mode_t mode = sb->st_mode;
    brojaci[UKU]++;
    if (S_ISREG(mode))
        brojaci[REG]++;
    else if (S_ISDIR(mode))
        brojaci[DIR]++;
    else if (S_ISCHR(mode))
        brojaci[CHR]++;
    else if (S_ISBLK(mode))
        brojaci[BLK]++;
    else if (S_ISFIFO(mode))
        brojaci[FIF]++;
    else if (S_ISLNK(mode))
        brojaci[LNK]++;
    else if (S_ISSOCK(mode))
        brojaci[SOC]++;

    return 0;
}

int main(int argc, char **argv)
{
    checkError(argc == 2, "args");
    for (int i = 0; i < 8; i++)
        brojaci[i] = 0;
    nftw(argv[1], obradi, 50, FTW_PHYS);
    for (int i = 0; i < 8; i++)
        printf("%d ", brojaci[i]);
    exit(EXIT_SUCCESS);
}