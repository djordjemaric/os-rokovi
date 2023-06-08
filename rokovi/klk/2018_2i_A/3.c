#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

    struct stat f_info;
    check_error(stat(argv[1], &f_info) != -1, "stat");

    mode_t prava_vlasnika = f_info.st_mode & S_IRWXU;
    mode_t prava_grupe = f_info.st_mode & S_IRWXG;
    mode_t prava_ostali = f_info.st_mode & S_IRWXO;
    prava_grupe >>= 3;
    prava_ostali <<= 3;
    mode_t nova_prava = prava_vlasnika | prava_grupe | prava_ostali;
    chmod(argv[1], nova_prava);
}