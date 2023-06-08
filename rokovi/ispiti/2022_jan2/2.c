#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
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

int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    
    char *new_file = malloc(strlen(argv[1]));
    check_error(new_file != NULL, "malloc");
    strcpy(new_file, argv[1]);

    char *ext = strrchr(argv[1], '.');
    check_error(ext != NULL, "nije .c");
    check_error(strcmp(ext,".c") == 0, "nije .c");
    new_file[strlen(new_file)-1] = 's';

    pid_t child_pid;
    child_pid = fork();
    check_error(child_pid != -1, "fork");
    if(child_pid == 0){
        //dete
        check_error(-1 != execlp("gcc", "gcc", "-std=c99", "-S", argv[1], "-o", new_file, NULL), "exec");
        exit(EXIT_FAILURE);//exec pojede, ne izvrsava se
    }
    int status = 0;
    check_error(waitpid(child_pid,&status,0) != -1, "waitpid");
    if(WIFEXITED(status)){
        if(WEXITSTATUS(status) != EXIT_SUCCESS)
        {
            free(new_file);
            exit(WEXITSTATUS(status));
        }
    }
    else{
        free(new_file);
        check_error(0,"fail");
    }

    FILE *f = fopen(new_file, "r");
    check_error(f != NULL, "fopen");
    char *linija = NULL;
    size_t size = 0;

    char trazeno[4] = "mov";
    int brojac = 0;

    while(getline(&linija, &size, f) != -1){
        char *nadjen = strstr(linija,trazeno);
        if(nadjen != NULL)
            brojac++;
    }

    fclose(f);
    free(linija);
    check_error(unlink(new_file) != -1, "unlink");
    free(new_file);

    printf("%d", brojac);
    exit(EXIT_SUCCESS);


}