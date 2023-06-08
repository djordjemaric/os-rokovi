#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <limits.h>
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
#define MAX_EVENTS (20)
#define WAIT_TIME (-1)

typedef struct
{
    int fd;
    int id;
} ep_data_t;
int main(int argc, char **argv)
{
    check_error(argc > 1, "args");

    int num_fifos = argc - 1;
    int ep_fd = epoll_create(num_fifos);
    check_error(ep_fd != -1, "epoll_create");

    FILE **streams = malloc(num_fifos * sizeof(FILE *));
    check_error(streams != NULL, "malloc");

    struct epoll_event current_event;
    for (int i = 0; i < num_fifos; i++)
    {
        int fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK, 0777);
        check_error(fd != -1, "open");

        ep_data_t *podaci = malloc(sizeof(ep_data_t));
        check_error(podaci != NULL, "malloc");
        podaci->fd = fd;
        podaci->id = i;
        streams[i] = fdopen(fd, "r");
        check_error(streams[i] != NULL, "fdopen");

        current_event.data.ptr = (void *)podaci;
        current_event.events = EPOLLIN | EPOLLHUP | EPOLLERR;

        check_error(epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &current_event) != -1, "epoll_ctl");
    }

    struct epoll_event *ready_events = malloc(MAX_EVENTS * sizeof(struct epoll_event));
    check_error(ready_events != NULL, "malloc");

    double max = -(__DBL_MAX__);
    int indeks = -1;
    while (num_fifos)
    {
        int num_ready;
        check_error((num_ready = epoll_wait(ep_fd, ready_events, MAX_EVENTS, WAIT_TIME)) != -1, "epoll_wait");
        for (int i = 0; i < num_ready; i++)
        {
            if (ready_events[i].events & EPOLLIN)
            {
                ep_data_t *pod = (ep_data_t *)ready_events[i].data.ptr;
                double broj, zbir = 0;
                while (fscanf(streams[pod->id], "%lf", &broj) == 1)
                    zbir += broj;
                if (zbir > max)
                {
                    max = zbir;
                    indeks = pod->id;
                }
            }
            else if (ready_events[i].events & (EPOLLERR | EPOLLHUP))
            {
                ep_data_t *pod = (ep_data_t *)ready_events[i].data.ptr;
                fclose(streams[pod->id]);
                free(pod);
                num_fifos--;
            }
        }
    }

    printf("%s", argv[indeks + 1]);
    free(ready_events);
    free(streams);
    close(ep_fd);

    exit(EXIT_SUCCESS);
}