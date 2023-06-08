#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
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

#define RD (0)
#define WR (1)
#define MAX_LEN (256)
#define BUFF_SIZE (1024)
int main(int argc, char **argv)
{
    check_error(argc == 2, "args");

    FILE *f = fopen(argv[1], "r");
    check_error(f != NULL, "open");

    char komanda[MAX_LEN];
    char argument[MAX_LEN];
    int bytes_read_max = 0;
    char max_komanda[MAX_LEN];
    char max_argument[MAX_LEN];
    while (fscanf(f, "%s %s", komanda, argument) == 2)
    {
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
                    check_error(0, "wait");
            }
            else
                check_error(0, "wait");

            int bytes_read_total = 0;
            int bytes_read = 0;
            char ispis[BUFF_SIZE];
            while ((bytes_read = read(pipe_fds[RD], ispis, BUFF_SIZE)) > 0)
                bytes_read_total += bytes_read;
            check_error(bytes_read != -1, "read");

            if (bytes_read_total > bytes_read_max)
            {
                bytes_read_max = bytes_read_total;
                strcpy(max_komanda, komanda);
                strcpy(max_argument, argument);
            }
            close(pipe_fds[RD]);
        }
        else
        {
            // dete
            close(pipe_fds[RD]);
            check_error(dup2(pipe_fds[WR], STDOUT_FILENO) != -1, "dup2");
            check_error(execlp(komanda, komanda, argument, NULL) != -1, "execlp");
            close(pipe_fds[WR]);
            // exit(EXIT_SUCCESS);
        }
    }

    printf("%s %s", max_komanda, max_argument);

    exit(EXIT_SUCCESS);
}