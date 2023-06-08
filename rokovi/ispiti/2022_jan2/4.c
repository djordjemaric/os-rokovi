#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <ctype.h>
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
#define MAX_EVENTS (256)
#define BUFF_SIZE (4096)

typedef struct{
    int fd;
    int id;
    int br_cif;
} ep_data_t;

int main(int argc, char **argv)
{
    check_error(argc >= 2, "args");
    unsigned num_fifos = argc - 1;
    int epoll_fd = epoll_create(num_fifos);
    check_error(epoll_fd != -1, "epoll_create");

    struct epoll_event current_event;
    int i = 0;
    for(i = 0; i < num_fifos; i++){
        memset(&current_event, 0, sizeof(struct epoll_event));
        int fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
        check_error(fd != -1, "open");

        ep_data_t *podaci = malloc(sizeof(ep_data_t));
        check_error(podaci != NULL, "malloc");
        podaci->fd = fd;
        podaci->id = i;
        podaci->br_cif = 0;

        current_event.data.ptr = podaci;
        current_event.events = EPOLLIN;

        check_error(-1 != epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &current_event), "epoll_ctl");
    }

    struct epoll_event ready_fds[MAX_EVENTS];
    int max_cif = 0;
    int max_id;
    while(num_fifos){
        int num_ready;
        check_error(-1 != (num_ready = epoll_wait(epoll_fd,ready_fds,MAX_EVENTS, -1)), "epoll_wait");

        for(i = 0; i < num_ready; i++){
            if(ready_fds[i].events & EPOLLIN){

                ep_data_t *podaci = (ep_data_t *)ready_fds[i].data.ptr;
                int fd = podaci->fd;
                int br_cif_tren = 0;
                char buff[BUFF_SIZE];
                int read_bytes = 0;
                while((read_bytes = read(fd,buff,BUFF_SIZE)) > 0){
                    check_error(read_bytes != -1, "read");
                    for(int j = 0; j < read_bytes; j++)
                    {
                        if(isdigit(buff[j]))
                            br_cif_tren++;
                    }
                }
                podaci->br_cif +=br_cif_tren;
                if(podaci->br_cif > max_cif)
                {
                    max_cif = podaci->br_cif;
                    max_id = podaci->id;
                }
            }else if(ready_fds[i].events & (EPOLLHUP | EPOLLERR)){
                ep_data_t *podaci = (ep_data_t *)ready_fds[i].data.ptr;
                close(podaci->fd);
                free(podaci);
                num_fifos--;
            }
        }
    }

    char *naziv = strrchr(argv[max_id+1],'/');
    printf("%s %d", naziv+1,max_cif);
    
    close(epoll_fd);

    exit(EXIT_SUCCESS);
}