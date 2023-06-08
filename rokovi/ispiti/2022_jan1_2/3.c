#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_LEN 1238

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

#define check_pthread(error,msg)\
    do{\
        int _err = error;\
        if(_err > 0){\
            errno = _err;\
            check_error(0,"msg");\
        }\
    }while(0)\

pthread_mutex_t global_lock;
int global_max_indeks = 0;
int global_max_reci = 0;

typedef struct{
    int id;
    char *path;
}thread_data_t;

typedef struct {
    int br_reci;
}thread_return_t;

thread_return_t thread_func(void *arg){
    thread_data_t *args = (thread_data_t *)arg;
    FILE *f = fopen(args->path, "r");
    check_error(NULL != f, "fopen");

    int brojac = 0;
    char s[MAX_LEN];
    while(fscanf(f,"%s", s) != EOF)
        brojac++;
    check_pthread(pthread_mutex_lock(&global_lock),"lock");
    if(brojac > global_max_reci){
        global_max_reci = brojac;
        global_max_indeks = args->id;
    }
    check_pthread(pthread_mutex_unlock(&global_lock),"unlock");

    thread_return_t *ret = malloc(sizeof(thread_return_t));
    check_error(ret != NULL, "malloc");
    fclose(f);
    return ret;
}

int main(int argc, char **argv)
{
    check_error(argc >= 2, "args");
    int n = argc - 1;

    pthread_t *tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");

    thread_data_t *data = malloc(n*szieof(thread_data_t));
    check_error(data != NULL, "malloc");


    check_pthread(pthread_mutex_init(&global_lock), "mutex_init");

    for(int i = 0; i < n; i++){
       data[i].id = i;
       data[i].path = argv[i+1];
       check_pthread(pthread_create(&tids[i], NULL, thread_func, (void*)&data[i]),"pthread_create");
    }

    int global_suma = 0;

    for(int i = 0; i < n; i++){
        thread_return_t *ret;
        check_pthread(pthread_join(tids[i], (void**)&ret),"pthread_join");
        global_suma = ret->br_reci;
        free(ret_val);
    }
    pthread_mutex_destroy(&global_lock);

    printf("%s %d", argv[global_max_indeks+1], global_suma);
    free(tids);
    free(data);
    exit(EXIT_SUCCESS);



}