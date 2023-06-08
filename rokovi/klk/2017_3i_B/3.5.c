#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
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

void walk(char *path, char *ext, int *br)
{
    struct stat f_info;
    check_error(lstat(path, &f_info) != -1, "lstat");
    if (S_ISREG(f_info.st_mode))
    {
        char *tmp = strrchr(path, '.');
        if (tmp != NULL)
        {
            if (!strcmp(tmp, ext))
                *br += 1;
        }
    }

    if (!S_ISDIR(f_info.st_mode))
        return;

    DIR *dir = opendir(path);
    check_error(dir != NULL, "opendir");

    struct dirent *cd;
    while ((cd = readdir(dir)) != NULL)
    {
        char *new_path = malloc(strlen(path) + strlen(cd->d_name) + 2);
        check_error(new_path != NULL, "malloc");
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, cd->d_name);

        if (!strcmp(cd->d_name, ".") || !strcmp(cd->d_name, ".."))
        {
            check_error(stat(new_path, &f_info) != -1, "...");
            free(new_path);
            continue;
        }
        walk(new_path, ext, br);
        free(new_path);
    }

    check_error(closedir(dir) != -1, "closedir");
}
int main(int argc, char **argv)
{
    check_error(argc == 3, "args");

    struct stat f_info;
    check_error(stat(argv[1], &f_info) != -1, "stat");
    check_error(S_ISDIR(f_info.st_mode), "nije dir");

    int brojac = 0;
    walk(argv[1], argv[2], &brojac);
    printf("%d", brojac);
    exit(EXIT_SUCCESS);
}