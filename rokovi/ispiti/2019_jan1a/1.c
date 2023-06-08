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

void ispisi_dan(struct tm *broken_time)
{
    switch (broken_time->tm_wday)
    {
    case 0:
        printf("nedelja ");
        break;
    case 1:
        printf("ponedeljak ");
        break;
    case 2:
        printf("utorak ");
        break;
    case 3:
        printf("sreda ");
        break;
    case 4:
        printf("cetvrtak ");
        break;
    case 5:
        printf("petak ");
        break;
    case 6:
        printf("subota ");
        break;
    default:
        check_error(0, "broken_time");
        break;
    }
}

int main(int argc, char **argv)
{
    check_error(argc == 3, "args");

    time_t unos = atoi(argv[1]);

    struct tm *broken_time = (localtime(&unos));
    ispisi_dan(broken_time);
    broken_time->tm_year += atoi(argv[2]);

    time_t pomereno = mktime(broken_time);
    broken_time = localtime(&pomereno);
    ispisi_dan(broken_time);
    exit(EXIT_SUCCESS);
}