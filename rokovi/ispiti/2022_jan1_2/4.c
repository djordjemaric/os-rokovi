#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
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
#define MAX_LEN 128
#define BUFF_SIZE 4096
int main(int argc, char **argv){
    int pipe_fds[2];
    int br_neuspesnih = 0;
    int max_ispis = 0;
    char max_komanda[MAX_LEN];

    int n;
    while(scanf("%d", &n) != EOF){
        //ucitavamo argumente
        char **args = malloc((n + 1) * sizeof(char*));
        check_error(args != NULL, "malloc");
        for(int i = 0; i < n; i++){
            args[i] = malloc(MAX_LEN);
            check_error(args[i] != NULL, "malloc");
            scanf("%s", args[i]);
        }
        //Zatvaramo vektor argumenata
        args[n] = NULL;
        //Kreiramo pajp za ovu komandu
        check_error(-1 != pipe(pipe_fds), "pipe");
        pid_t child = fork();
        
        if(child == 0){
            //u detetu smo
            close(pipe_fds[RD]);
            check_error(-1 != dup2(pipe_fds[WR],STDOUT_FILENO), "dup2");//preusmeravamo izlaz
            check_error(execvp(args[0],args),"exec");
            exit(EXIT_FAILURE);//pojede exec, ne izvrsava se
        }else{
            close(pipe_fds[WR]);
            int status = 0;
            check_error(waitpid(child,&status,0),"waitpid");

            if(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS){
                    int brojac = 0;
                    int read_bytes = 0;
                    char buff[BUFF_SIZE];
                    while((read_bytes = read(pipe_fds,buff, BUFF_SIZE)) > 0){
                        check_error(read_bytes != -1, "read");
                        brojac += read_bytes;
                    }
                    if(brojac > max_ispis){
                        max_ispis = brojac;
                        strcpy(max_komanda,)
                    }
                }
            }else{
                br_neuspesnih++;
            }
        }

    }
}