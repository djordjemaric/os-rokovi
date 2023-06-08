#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
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

int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    struct stat file_info;
    check_error(-1 != lstat(argv[1], &file_info), "lstat");

    struct passwd *korisnik = getpwuid(file_info.st_uid);
    check_error(korisnik != NULL, "getpwuid");
    struct group  *grupa = getgrgid(file_info.st_gid);
    check_error(grupa != NULL, "getgruid");
    printf("%s %s", korisnik->pw_name, grupa->gr_name);
    exit(EXIT_SUCCESS);
}