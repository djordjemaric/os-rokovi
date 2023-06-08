#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
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

#define MAX_LEN (256)

int proveri_rec(char *s){
    int i = 0;
    int odgovara = 1;
    while(s[i]){
        if(!isupper(s[i]))
        {
            odgovara = 0;
            break;
        }
        i++;
    }
    return odgovara;
}

int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    struct stat file_info;
    // check_error(-1 != stat(argv[1],&file_info), "stat");
    // check_error(S_ISREG(file_info.st_mode), "not a reg file");

    printf("ovde\n");
    int fd = open(argv[1], O_RDWR);
    check_error(-1 != fd, "open");
    FILE *f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");
    printf("ovde\n");
    char rec[MAX_LEN];
    struct flock lock;
    int izmenjenih = 0;
    while(fscanf(f,"%s", rec) == 1){
        if(proveri_rec(rec)){


        int duzina = strlen(rec);
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = ftell(f);
        lock.l_len = duzina;    
        errno = 0;
        int ret_val = fcntl(fd, F_SETLK, &lock);

        if(ret_val != -1){
            for(int i = 0; rec[i]; i++)
                rec[i] = '#';
            izmenjenih += duzina;
            fseek(f,-duzina,SEEK_CUR);
            fprintf(f,"%s", rec);
            lock.l_type = F_UNLCK;
            check_error(fcntl(fd,F_SETLK,&lock) != -1, "fcntl");
            
        }else{
            if(errno != EACCES && errno != EAGAIN)
                check_error(0, "fail");
            continue;            
        }
    }
    }
    printf("%d", izmenjenih);
}