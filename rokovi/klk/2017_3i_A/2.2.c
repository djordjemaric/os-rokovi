#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#define checkError(cond,msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)
//Ispisi ime vlasnika fajla i njegovu grupu
void printOwnerInfo(const char *path)
{
    //Uzimamo podatke o fajlu
    struct stat fInfo;
    checkError(stat(path, &fInfo) == 0, "stat");
    //Uzimamo podatke o vlasniku na osnovu uid
    struct passwd* vlasnik = getpwuid(fInfo.st_uid);
    checkError(vlasnik != NULL, "getpwuid");
    printf("%s ", vlasnik->pw_name);
    //Uzimamo podatke o grupi na osnovu gid
    struct group *grupa = getgrgid(fInfo.st_gid);
    checkError(grupa != NULL, "getgrgid");
    printf("%s", grupa->gr_name);

}

int main(int argc, char **argv)
{
    checkError(argc == 2, "argc");
    printOwnerInfo(argv[1]);
    return 0;

}