#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define RD_END 0
#define WR_END 1
#define BUFF_SIZE 4096

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\


int main(int argc, char **argv){
    check_error(argc == 1, "args");

    int pipe_fds[2];
    check_error(-1 != pipe(pipe_fds), "pipe");
    pid_t child_pid = fork();
    check_error(child_pid != -1, "fork");
    if(child_pid != 0){
        //roditelj
        close(pipe_fds[WR_END]);
        int bytes_read;
        char buff[BUFF_SIZE];

        bytes_read = read(pipe_fds[RD_END],buff,BUFF_SIZE);
        check_error(bytes_read != -1, "read");

        char *korisnicki_deo = strstr(buff,"home/");
        if(korisnicki_deo != NULL){
            korisnicki_deo = korisnicki_deo+5;
            korisnicki_deo = strtok(korisnicki_deo, "/");
            printf("%s", korisnicki_deo);
        }else{
            printf("system\n");
        }
        close(pipe_fds[RD_END]);
        exit(EXIT_SUCCESS);
    }
    else{
        //dete
        close(pipe_fds[RD_END]);
        check_error(-1 != dup2(pipe_fds[WR_END], STDOUT_FILENO), "dup2");
        check_error(-1 != execlp("pwd", "pwd", NULL), "exec");
        // close(pipe_fds[WR_END]);
        // exit(EXIT_SUCCESS);
    }
}