#define _DEFAULT_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

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

bool should_terminate = false;
bool sigusr1_arrived = false;
bool sigusr2_arrived = false;

void sighandler(int signum)
{
    switch (signum)
    {
    case SIGUSR1:
        sigusr1_arrived = true;
        break;
    case SIGUSR2:
        sigusr2_arrived = true;
        break;
    case SIGTERM:
        should_terminate = true;
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

    int broj;
    do
    {
        pause();
        scanf("%d", &broj);
        if (sigusr1_arrived)
        {
            sigusr1_arrived = false;
            if (broj < 0)
                broj *= -1;
            printf("%d\n", broj);
        }
        else if (sigusr2_arrived)
        {
            sigusr2_arrived = false;
            broj *= broj;
            printf("%d\n", broj);
        }

    } while (!should_terminate);
    exit(EXIT_SUCCESS);
}