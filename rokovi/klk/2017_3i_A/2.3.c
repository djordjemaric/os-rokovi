#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#define checkError(cond,msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

void makeDir(const char *path, char *mode)
{
    mode_t prava = strtol(mode,NULL,8);
    mode_t oldUmask = umask(0);
    errno = 0;
    //Ukoliko je mkdir vratio gresku razlicitu od EEXIST(fajl postoji), izadji
    checkError(mkdir(path,prava) == 0 || errno == EEXIST, "mkdir");
    if(errno != 0)
        checkError(chmod(path,prava) == 0, "chmod");
    umask(oldUmask);    
}

void makeFile(const char* path, char *mode)
{
    mode_t prava = strtol(mode,NULL,8);
    mode_t oldUmask = umask(0);
    int fd;
    errno = 0;
    //Ukoliko je open vratio gresku razlicitu od EEXIST(fajl postoji), izadji
    checkError((fd = open(path, O_CREAT | O_RDWR | O_EXCL, prava)) != -1 || errno == EEXIST, "open");
    //Ako errno nije 0, mora biti EEXIST, pa promeni prava pristupa
    if(errno != 0)
        checkError(chmod(path,prava) == 0, "chmod");
    else
        close(fd);
    umask(oldUmask);
}
//Napravi trazeni tip fajla ukoliko ne postoji, a ako postoji, promeni mu prava pristupa
int main(int argc, char **argv)
{
    checkError(argc == 4, "argv");
    
    switch(argv[1][1])
    {
        case 'f':
            makeFile(argv[2],argv[3]);
            break;
        case 'd':
            makeDir(argv[2],argv[3]);
        default:
            break;
    }
    return 0;    
}