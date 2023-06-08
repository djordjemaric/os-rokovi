#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
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

void ispisi_mesec(int mesec)
{
    switch (mesec)
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
        break;
    case 11:
        printf("decembar ");
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    check_error(argc == 3, "args");
    time_t ulazno_vreme = atol(argv[1]);
    int pomeraj = atoi(argv[2]);
    struct tm *broken_time = localtime(&ulazno_vreme);
    ispisi_mesec(broken_time->tm_mon);
    broken_time->tm_mday += pomeraj;
    time_t pomereno = mktime(broken_time);
    broken_time = localtime(&pomereno);
    ispisi_mesec(broken_time->tm_mon);
    exit(EXIT_SUCCESS);
}