#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#define MAX_LEN 256
#define BUFF_SIZE 4095

#define RD 0
#define WR 1
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

int main(int argc, char **argv)
{
    check_error(argc == 2, "args");

    FILE *f_ulaz = fopen(argv[1], "r");
    check_error(f_ulaz != NULL, "fopen");

    FILE *f_izlaz = fopen("errors.txt", "w");
    check_error(f_izlaz != NULL, "fopen");

    char komanda[MAX_LEN];
    char argument[MAX_LEN];
    while (fscanf(f_ulaz, "%s %s", komanda, argument) == 2)
    {

        int pipe_fds[2];
        check_error(pipe(pipe_fds) != -1, "pipe");

        pid_t child_pid = fork();
        check_error(child_pid != -1, "fork");

        if (child_pid > 0) // parent
        {
            close(pipe_fds[WR]);

            int status = 0;
            check_error(waitpid(child_pid, &status, 0) != -1, "waitpid");
            if (WIFEXITED(status))
            {
                if (WEXITSTATUS(status) != EXIT_SUCCESS)
                {
                    FILE *pajp = fdopen(pipe_fds[RD], "r");
                    check_error(pajp != NULL, "fdopen");
                    char izlaz_komande[BUFF_SIZE];
                    while (fscanf(pajp, "%s", izlaz_komande) == 1)
                        fprintf(f_izlaz, "%s ", izlaz_komande);
                    fprintf(f_izlaz, "\n");
                    fclose(pajp);
                }
                else
                    close(pipe_fds[RD]);
            }
        }
        else // child
        {
            close(pipe_fds[RD]);
            check_error(dup2(pipe_fds[WR], STDERR_FILENO) != -1, "dup2");
            check_error(execlp(komanda, komanda, argument, NULL) != -1, "exec");
            close(pipe_fds[WR]);
            exit(EXIT_SUCCESS);
        }
    }
    exit(EXIT_SUCCESS);
}