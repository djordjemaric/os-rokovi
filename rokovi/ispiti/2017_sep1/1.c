#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
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
#define UNUSED(x) ((void)x);
#define MAX_LEN 64
bool usr1_recieved = false;
bool usr2_recieved = false;
bool terminate = false;

void sighandler(int signum)
{
    switch (signum)
    {
    case SIGUSR1:
        usr1_recieved = true;
        break;
    case SIGUSR2:
        usr2_recieved = true;
        break;
    case SIGTERM:
        terminate = true;
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    check_error(signal(SIGUSR1, sighandler) != SIG_ERR, "signal");
    check_error(signal(SIGUSR2, sighandler) != SIG_ERR, "signal");
    check_error(signal(SIGTERM, sighandler) != SIG_ERR, "signal");

    do
    {
        char rec[MAX_LEN];
        scanf("%s", rec);
        pause();
        if (usr1_recieved)
        {
            // ispisi unazad
            int d = strlen(rec);
            for (int i = d - 1; i >= 0; i--)
                printf("%c", rec[i]);
            printf("\n");
            usr1_recieved = false;
        }
        if (usr2_recieved)
        {
            // mala postaju velika
            int d = strlen(rec);
            for (int i = 0; i < d; i++)
            {
                if (islower(rec[i]))
                    printf("%c", toupper(rec[i]));
                else
                    printf("%c", rec[i]);
            }
            printf("\n");
            usr2_recieved = false;
        }
    } while (!terminate);

    exit(EXIT_SUCCESS);
}
