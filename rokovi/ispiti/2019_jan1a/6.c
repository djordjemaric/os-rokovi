#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
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

void walk(char *path, char *ext_stara, char *ext_nova)
{
    struct stat file_info;
    check_error(lstat(path, &file_info) != -1, "lstat");

    if (S_ISREG(file_info.st_mode))
    {
        char *tmp = strrchr(path, '.');
        if (tmp != NULL)
        {
            if (!strcmp(tmp, ext_stara))
            {
                char kopija[strlen(path) + 1];
                strcpy(kopija, path);
                kopija[strlen(path) - strlen(ext_stara)] = '\0';
                strcat(kopija, ext_nova);
                rename(path, kopija);
            }
        }
        return;
    }

    if (!S_ISDIR(file_info.st_mode))
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
            free(new_path);
            continue;
        }
        walk(new_path, ext_stara, ext_nova);
        free(new_path);
    }
    check_error(closedir(dir) != -1, "closedir");
}

int main(int argc, char **argv)
{
    check_error(argc == 4, "args");
    struct stat file_info;
    check_error(stat(argv[1], &file_info) != -1, "stat");
    check_error(S_ISDIR(file_info.st_mode), "not a dir");
    walk(argv[1], argv[2], argv[3]);
    exit(EXIT_SUCCESS);
}