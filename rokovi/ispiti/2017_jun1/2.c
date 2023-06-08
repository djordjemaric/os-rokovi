#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

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

#define RD 0
#define WR 1
int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    int pipe_fds[2];
    check_error(pipe(pipe_fds) != -1, "pipe");
    pid_t child_pid = fork();
    check_error(child_pid != -1, "fork");
    if (child_pid > 0)
    {
        // roditelj
        close(pipe_fds[WR]);
        int status = 0;
        check_error(waitpid(child_pid, &status, 0) != -1, "waitpid");
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) != EXIT_SUCCESS)
            {
                printf("Neuspeh");
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            printf("Neuspeh");
            exit(EXIT_SUCCESS);
        }

        FILE *f = fdopen(pipe_fds[RD], "r");
        check_error(f != NULL, "fdopen");
        char ispis[11];
        fscanf(f, "%s", ispis);
        ispis[10] = '\0';
        printf("%s", ispis);
        fclose(f);
    }
    else
    {
        // dete
        close(pipe_fds[RD]);
        check_error(-1 != dup2(pipe_fds[WR], STDOUT_FILENO), "dup2");
        check_error(execlp("ls", "ls", "-l", argv[1], NULL), "execlp");
        close(pipe_fds[WR]);
        exit(EXIT_SUCCESS); // nikad
    }
    exit(EXIT_SUCCESS);
}