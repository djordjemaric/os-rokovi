#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>
#include <time.h>
#include <string.h>
#define DAY_SEC (24 * 60 * 60)
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
char *trazena_ext;
int izdvoji(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F)
    {
        time_t vreme = sb->st_mtime;
        time_t now = time(NULL);
        time_t razlika = now - razlika;
        int broj_dana = razlika / DAY_SEC;
        if (broj_dana <= 7)
        {
            char *ext = strrchr(fpath, '.');
            if (ext != NULL)
            {
                if (!strcmp(ext, trazena_ext))
                {
                    char *file_name = strrchr(fpath, '/');
                    if (file_name == NULL)
                        printf("%s ", fpath);
                    else
                        printf("%s ", file_name + 1);
                }
            }
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    check_error(argc == 3, "args");

    struct stat f_info;
    check_error(stat(argv[1], &f_info) != -1, "stat");
    check_error(S_ISDIR(f_info.st_mode), "nije direktorijum");
    trazena_ext = argv[2];
    nftw(argv[1], izdvoji, 50, 0);
    exit(EXIT_SUCCESS);
}