#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define checkError(cond,msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

int main(int argc, char **argv){
    checkError(argc == 2, "argv");
    struct stat fileInfo;
    checkError(stat(argv[1],&fileInfo) == 0,"stat");
    //Ispisi prava pristupa za fajl
    char prava[11] = {"drwxrwxrwx"};
    if(S_ISREG(fileInfo.st_mode))
        prava[0] = '-';
    if(!(fileInfo.st_mode & S_IRUSR))
        prava[1] = '-';
    if(!(fileInfo.st_mode & S_IWUSR))
       prava[2] = '-';
    if(!(fileInfo.st_mode & S_IXUSR))
        prava[3] = '-';

    if(!(fileInfo.st_mode & S_IRGRP))
        prava[4] = '-';
    if(!(fileInfo.st_mode & S_IWGRP))
        prava[5] = '-';
    if(!(fileInfo.st_mode & S_IXGRP))
        prava[6] = '-';

    if(!(fileInfo.st_mode & S_IROTH))
        prava[7] = '-';
    if(!(fileInfo.st_mode & S_IWOTH))
        prava[8] = '-';
    if(!(fileInfo.st_mode & S_IXOTH))
        prava[9] = '-';

    printf("%s", prava);

    return 0;
}
