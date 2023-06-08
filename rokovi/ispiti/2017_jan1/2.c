#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

#define RD_END 0
#define WR_END 1

int main(int argc, char **argv)
{
    checkError(argc == 2, "args");

    int pipeFds[2];
    checkError(pipe(pipeFds) != -1, "pipe fail");
    pid_t child_pid = fork();
    // roditelj
    if (child_pid > 0)
    {
        checkError(close(pipeFds[WR_END]) != -1, "pipe fail");
        close(pipeFds[RD_END]);

    } //dete
    else
    {
        checkError(close(pipeFds[RD_END]) != -1, "pipe fail");
        checkError(dup2(STDOUT_FILENO, pipeFds[WR_END]) != -1, "dup2 fail");

        struct stat fileInfo;
        checkError(stat(argv[1], &fileInfo) != -1, "stat fail");
        printf("%u", fileInfo.st_size);
        exit(EXIT_SUCCESS);
    }
    int status;
    checkError(wait(&status) != -1, "wait");
    if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status) != EXIT_SUCCESS)
        {
            printf("Neuspeh\n");
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        printf("Neuspeh\n");
        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}