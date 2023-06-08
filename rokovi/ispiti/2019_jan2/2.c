#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
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
#define BUFF_SIZE (1024)
int main(int argc, char **argv)
{
    check_error(argc > 1, "args");

    for (int i = 1; i < argc; i++)
    {
        int pipe_fds[2];
        check_error(pipe(pipe_fds) != -1, "pipe");

        pid_t child_pid = fork();
        check_error(child_pid != -1, "fork");

        if (child_pid > 0)
        { // roditelj
            int status = 0;
            waitpid(child_pid, &status, 0);
            close(pipe_fds[WR]);
            if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
            {
                char buff[BUFF_SIZE];
                int read_bytes;
                while ((read_bytes = read(pipe_fds[RD], buff, BUFF_SIZE)) > 0)
                    check_error(read_bytes != -1, "read");

                char rezultat[BUFF_SIZE];
                sscanf(buff, "%s", rezultat);
                printf("%s ", rezultat);
            }
            else
            {
                printf("neuspeh ");
            }
        }
        else
        { // dete
            close(pipe_fds[RD]);
            check_error(dup2(pipe_fds[WR], STDOUT_FILENO) != -1, "dup2");
            struct stat f_info;
            if (stat(argv[i], &f_info) == -1)
                exit(EXIT_FAILURE);
            check_error(execlp("du", "du", "-sch", argv[i], NULL) != -1, "execlp");
            close(pipe_fds[WR]);
            exit(EXIT_SUCCESS);
        }
    }
    exit(EXIT_SUCCESS);
}