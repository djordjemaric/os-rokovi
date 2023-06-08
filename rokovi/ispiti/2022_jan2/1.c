#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>
#include <time.h>
#include <limits.h>
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

void ispisi_dan(struct tm *broken_time){
    switch (broken_time->tm_wday)
    {
    case 0:
        printf("nedelja");
        break;
    case 1:
        printf("ponedeljak ");
        break;
    case 2:
        printf("utorak ");
        break;
    case 3:
        printf("sreda ");
        break;
    case 4:
        printf("cetvrtak ");
        break;
    case 5:
        printf("petak ");
        break;
    case 6:
        printf("subota ");
        break;
    default:
        check_error(0, "broken_time");
        break;
    }
}

int proveri(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    if(typeflag == FTW_F){
        char *path = realpath(fpath, NULL);
        check_error(path != NULL, "realpath");
        printf("%s ", path);
        time_t mod_vreme = sb->st_mtime;
        struct tm *broken_time;
        check_error((broken_time = localtime(&mod_vreme)) != NULL,"localtime");
        ispisi_dan(broken_time);
        printf("\n");
    }
    return 0;
}

int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    struct stat file_info;
    check_error(-1 != stat(argv[1], &file_info), "stat");
    check_error(S_ISDIR(file_info.st_mode), "not a dir");
    check_error(-1 != nftw(argv[1],proveri, 50,0), "nftw");
    exit(EXIT_SUCCESS);
}