#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#define unused(x) ((void)x)

#define checkError(cond, msg)                     \
    do                                            \
    {                                             \
        if (!(cond))                              \
        {                                         \
            perror(msg);                          \
            fprintf("%s:%d", __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                   \
        }                                         \
    } while (0)
int sigu1_count = 0, sigu2_count = 0;
bool terminate = false;
void osSIGU1handler(int signum)
{
    unused(signum);
    sigu2_count++;
}
void osSIGU2handler(int signum)
{
    unused(signum);
    sigu2_count++;
}
void osSIGTERMhandler(int signum)
{
    unused(signum);
    terminate = true;
    printf("%d %d", sigu1_count, sigu2_count);
}

int main(int argc, char **argv)
{
    checkError(SIG_ERR != signal(SIGUSR1, osSIGU1handler), "sigusr1handler set failed");
    checkError(SIG_ERR != signal(SIGUSR2, osSIGU2handler), "sigusr2handler set failed");
    checkError(SIG_ERR != signal(SIGTERM, osSIGTERMhandler), "sigterm set failed");

    do
    {
        pause();
    } while (!terminate);
    return 0;
}
