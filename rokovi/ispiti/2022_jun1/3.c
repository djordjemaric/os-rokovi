#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LEN 128
#define UNUSED(x) ((void)x)
#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\

bool usr1_set = false;
bool usr2_set = false;
bool quit_set = false;
void sighandler(int signum){
    switch(signum){
    case SIGUSR1:
        usr1_set = true;
        break;
    case SIGUSR2:
        usr2_set = true;
        break;
    case SIGQUIT:
        quit_set = true;
        break;
    }

}

int main(int argc, char **argv){
    UNUSED(argc);
    UNUSED(argv);
    check_error(SIG_ERR != signal(SIGUSR1,sighandler), "signal");
    check_error(SIG_ERR != signal(SIGUSR2,sighandler), "signal");
    check_error(SIG_ERR != signal(SIGQUIT,sighandler), "signal");

    while(1){
        char rec[MAX_LEN];
        scanf("%s", rec);
        pause();
        if(quit_set)
            break;
        if(usr1_set){
            //palindrom 0/1
            int d = strlen(rec);
            d--;
            int l = 0, pal = 1;
            while(l < d){
                if(rec[l] != rec[d])
                {
                    pal = 0;
                    break;
                }
                l++;
                d--;
            }   
            printf("%d\n", pal);
            usr1_set = false;
        }
        if(usr2_set){
            int d = strlen(rec);
            for(int i = d-1;i>=0;i--)
                printf("%c", rec[i]);
            printf("\n");
            usr2_set = false;
        }
    }
    exit(EXIT_SUCCESS);
}