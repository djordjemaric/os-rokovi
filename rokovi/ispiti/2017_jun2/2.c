#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
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
    int brojac = 0;
    if (child_pid > 0)
    {
        // roditelj
        close(pipe_fds[WR]);
        FILE *f;
        check_error((f = fdopen(pipe_fds[RD], "r")) != NULL, "fdopen");
        char *linija = NULL;
        size_t size = 0;
        while (getline(&linija, &size, f) != EOF)
        {
            char *ekstenzija = strrchr(linija, '.');
            if (ekstenzija != NULL)
            {

                ekstenzija[strlen(ekstenzija) - 1] = '\0';
                if (!strcmp(ekstenzija, argv[1]))
                    brojac++;
            }
        }
        free(linija);
        fclose(f);
    }
    else
    {
        // dete
        close(pipe_fds[RD]);

        check_error(dup2(pipe_fds[WR], STDOUT_FILENO) != -1, "dup2");
        check_error(execlp("find", "find", ".", "-type", "f", NULL) != -1, "execlp");
        close(pipe_fds[WR]);
        exit(EXIT_FAILURE);
    }
    int status = 0;
    check_error(waitpid(child_pid, &status, 0) != -1, "wait");
    printf("%d", brojac);

    exit(EXIT_SUCCESS);
}