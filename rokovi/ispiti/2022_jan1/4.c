#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
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

#define RD 0
#define WR 1
#define MAX_LEN 1024

char **spakuj_argumente(char *linija)
{
    int read_bytes_total = 0, read_bytes;
    int n = 0;
    sscanf(linija, "%d%n", &n, &read_bytes);
    read_bytes_total += read_bytes;

    char **argumenti = malloc((n + 2) * sizeof(char *));
    check_error(argumenti != NULL, "malloc");

    char rec[MAX_LEN];
    // naziv komande
    sscanf(linija + read_bytes_total, "%s%n", rec, &read_bytes);
    read_bytes_total += read_bytes;

    argumenti[0] = malloc(strlen(rec) + 1);
    check_error(argumenti[0] != NULL, "malloc");
    argumenti[1] = malloc(strlen(rec) + 1);
    check_error(argumenti[1] != NULL, "malloc");

    strcpy(argumenti[0], rec);
    strcpy(argumenti[1], rec);

    for (int i = 1; i < n; i++)
    {
        sscanf(linija + read_bytes_total, "%s%n", rec, &read_bytes);
        read_bytes_total += read_bytes;
        argumenti[i + 1] = malloc(strlen(rec) + 1);
        check_error(argumenti[i + 1] != NULL, "malloc");
        strcpy(argumenti[i + 1], rec);
    }
    argumenti[n + 1] = NULL;

    return argumenti;
}
void ispisi(char **argumenti)
{
    for (int i = 0; argumenti[i] != NULL; i++)
        printf("%s ", argumenti[i]);
    printf("\n");
}
void oslobodi(char **argumenti)
{
    for (int i = 0; argumenti[i] != NULL; i++)
        free(argumenti[i]);
    free(argumenti);
}

int main(int argc, char **argv)
{
    char *linija = NULL;
    size_t size = 0;
    int max_procitano = 0;
    char max_komanda[MAX_LEN];
    int br_neuspelih = 0;
    int br_uspelih = 0;

    while (getline(&linija, &size, stdin) > 0)
    {
        char **argumenti = spakuj_argumente(linija);

        int c2p[2];
        check_error(pipe(c2p) != -1, "pipe");

        pid_t child_pid = fork();
        check_error(child_pid != -1, "fork");

        if (child_pid == 0)
        {
            // dete
            close(c2p[RD]);
            check_error(dup2(c2p[WR], STDOUT_FILENO) != -1, "dup2");
            check_error(execvp(argumenti[0], argumenti + 1) != -1, "execvp");
        }

        // roditelj
        close(c2p[WR]);
        int status = 0;
        check_error(waitpid(child_pid, &status, 0) != -1, "wait");

        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
        {
            char buff[MAX_LEN];
            int read_total = 0;
            int read_bytes = 0;
            br_uspelih++;
            while ((read_bytes = read(c2p[RD], buff, MAX_LEN)) > 0)
                read_total += read_bytes;

            if (read_total > max_procitano)
            {
                max_procitano = read_total;
                strcpy(max_komanda, argumenti[0]);
            }
        }
        else
        {
            br_neuspelih++;
        }
        oslobodi(argumenti);
        close(c2p[RD]);
    }

    printf("%d %s", br_neuspelih, br_uspelih > 0 ? max_komanda : "nema");

    free(linija);
    exit(EXIT_SUCCESS);
}