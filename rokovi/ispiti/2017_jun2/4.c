#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <limits.h>
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

int main(int argc, char **argv)
{
    check_error(argc > 1, "args");
    int num_of_fifos = argc - 1;

    struct pollfd *monitors = malloc(num_of_fifos * sizeof(struct pollfd));
    check_error(monitors != NULL, "malloc");
    FILE **streams = malloc(num_of_fifos * sizeof(FILE *));
    check_error(streams != NULL, "malloc");

    for (int i = 0; i < num_of_fifos; i++)
    {
        monitors[i].fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(monitors[i].fd != -1, "open");
        monitors[i].events = POLLIN | POLLHUP | POLLERR;
        monitors[i].revents = 0;
        streams[i] = fdopen(monitors[i].fd, "r");
        check_error(streams[i] != NULL, "fdopen");
    }

    int br_aktivnih = num_of_fifos;
    int max = INT_MIN;
    char *putanja = NULL;
    while (br_aktivnih)
    {
        int pol = poll(monitors, num_of_fifos, -1);
        check_error(pol != -1, "poll");
        for (int i = 0; i < num_of_fifos; i++)
        {
            if (monitors[i].revents & POLLIN)
            {
                int broj;
                while (fscanf(streams[i], "%d", &broj) != EOF)
                {

                    if (broj > max)
                    {
                        max = broj;
                        putanja = argv[i + 1];
                    }
                }
                monitors[i].revents = 0;
            }
            else if (monitors[i].revents & (POLLHUP | POLLERR))
            {
                br_aktivnih--;
                monitors[i].fd = -1;
                monitors[i].events = 0;
                monitors[i].revents = 0;
                fclose(streams[i]);
            }
        }
    }

    printf("%d ", max);
    char *ispis = strrchr(putanja, '/');
    if (ispis != NULL)
        printf("%s", ispis + 1);
    else
        printf("%s", putanja);
    free(monitors);
    free(streams);
    exit(EXIT_SUCCESS);
}