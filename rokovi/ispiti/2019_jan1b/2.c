#define _DEFAULT_SOURCE
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

int skor = 0;
bool terminate = false;
void sig_handler(int signum)
{
    switch (signum)
    {
    case SIGUSR1:
        skor += 1;
        break;
    case SIGUSR2:
        skor += 2;
        break;
    case SIGINT:
        skor -= 4;
        break;
    case SIGTERM:
        terminate = true;
        break;
    }
}
int main(int argc, char **argv)
{
    check_error(signal(SIGUSR1, sig_handler) != SIG_ERR, "signal");
    check_error(signal(SIGUSR2, sig_handler) != SIG_ERR, "signal");
    check_error(signal(SIGINT, sig_handler) != SIG_ERR, "signal");
    check_error(signal(SIGTERM, sig_handler) != SIG_ERR, "signal");

    do
    {
        pause();
    } while (!terminate);
    printf("%d", skor);
    return 0;
}