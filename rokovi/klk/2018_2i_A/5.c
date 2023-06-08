#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#define checkError(cond, msg)                             \
    do                                                    \
    {                                                     \
        if (!(cond))                                      \
        {                                                 \
            perror(msg);                                  \
            fprintf(stderr, "%s:%d", __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                           \
        }                                                 \
    } while (0)

void walk(char *path, unsigned *min, unsigned *max)
{
    struct stat f_info;
    checkError(lstat(path, &f_info) != -1, "lstat");

    if (S_ISREG(f_info.st_mode))
    {
        if (f_info.st_size < *min)
            *min = f_info.st_size;
        if (f_info.st_size > *max)
            *max = f_info.st_size;
    }
    if (!S_ISDIR(f_info.st_mode))
        return;

    DIR *dir = opendir(path);
    checkError(dir != NULL, "opendir");

    struct dirent *cd;
    while ((cd = readdir(dir)) != NULL)
    {
        char *new_path = malloc(strlen(path) + strlen(cd->d_name) + 2);
        checkError(new_path != NULL, "malloc");
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, cd->d_name);

        if (!strcmp(cd->d_name, ".") || !strcmp(cd->d_name, ".."))
        {
            checkError(stat(path, &f_info) != -1, "...");
            free(new_path);
            continue;
        }

        walk(new_path, min, max);
        free(new_path);
    }
    checkError(closedir(dir) != -1, "closedir");
}

int main(int argc, char **argv)
{
    checkError(argc == 2, "args");

    struct stat fInfo;
    checkError(stat(argv[1], &fInfo) != -1, "stat");
    checkError(S_ISDIR(fInfo.st_mode), "not a dir");

    unsigned min = UINT_MAX, max = 0;
    walk(argv[1], &min, &max);
    printf("%u", (max - min));
    exit(EXIT_SUCCESS);
}