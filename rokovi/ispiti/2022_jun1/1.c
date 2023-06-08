#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define check_error(cond,msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            fprintf(stderr, "File: %s, function: %s, line: %d", __FILE__, __func__, __LINE__);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\

void ispisi_mesec(struct tm *broken_time)
{
    switch (broken_time->tm_mon)
    {
    case 0:
        printf("januar ");
        break;
    case 1:
        printf("februar ");
        break;
    case 2:
        printf("mart ");
        break;
    case 3:
        printf("april ");
        break;
    case 4:
        printf("maj ");
        break;
    case 5:
        printf("jun ");
        break;
    case 6:
        printf("jul ");
        break;
    case 7:
        printf("avgust ");
        break;
    case 8:
        printf("septembar ");
        break;
    case 9:
        printf("oktobar ");
        break;
    case 10:
        printf("novembar ");
    case 11:
        printf("decembar ");
        break;
    default:
        check_error(0, "broken_time");
        break;
    }
}
int main(int argc, char **argv){
    check_error(argc == 3, "args");
    
    time_t sekunde = atoi(argv[1]);

    struct tm *broken_time;
    check_error((broken_time = localtime(&sekunde)) != NULL, "localtime");
    ispisi_mesec(broken_time);
    int pomeraj = atoi(argv[2]);
    broken_time->tm_mon -= pomeraj;
    check_error((sekunde = mktime(broken_time)) != -1, "mktime");
    check_error((broken_time = localtime(&sekunde)) != NULL, "localtime");
    ispisi_mesec(broken_time);
    
    exit(EXIT_SUCCESS);


}