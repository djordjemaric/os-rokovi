#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <pthread.h>
#include <errno.h>
#include <stdbool.h>
#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\

#define check_pthread(error, msg)\
    do{\
        int _err = error;\
        if(_err > 0){\
            errno = _err;\
            check_error(false, msg);\
        }\
    }while(0)\

int global_max = 0;
pthread_mutex_t global_lock;
int **matrica;
typedef struct{
    int red;
    int id;
} thread_data_t;

void *thread_func(void *arg){
    thread_data_t *args = (thread_data_t *)arg;
    int red = args->red;
    int m = matrica[red][0];
    int maks = 0;
    for(int j = 1; j <= m; j++)
    {
        if(matrica[red][j] > maks)
            maks = matrica[red][j];
    }
    pthread_mutex_lock(&global_lock);
    if(maks>global_max)
        global_max = maks;
    pthread_mutex_unlock(&global_lock);
    return NULL;
}

int main(int argc, char **argv){
    check_error(argc == 2, "args");
    //Otvaramo fajl
    FILE *f = fopen(argv[1],"r");
    check_error(f != NULL, "fdopen");
    
    //Broj redova i alociranje matrice
    int n;
    fscanf(f, "%d", &n);
    matrica = malloc(n*sizeof(int*));
    check_error(matrica != NULL, "malloc");

    for(int i = 0; i < n; i++){
        int m;
        fscanf(f, "%d", &m);
        matrica[i] = malloc((m+1)*sizeof(int));
        check_error(matrica[i] != NULL, "malloc");
        
        matrica[i][0] = m;
        for(int j = 1; j <= m;j++)
            fscanf(f, "%d", &matrica[i][j]);
    }

    thread_data_t *data = malloc(n*sizeof(thread_data_t));
    check_error(data != NULL, "malloc");

    pthread_t *tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");

    check_pthread(pthread_mutex_init(&global_lock, NULL), "mutex_init");
    for(int i = 0; i < n;i++){
        data[i].red = i;
        data[i].id = i;

        check_pthread(pthread_create(&tids[i],NULL,thread_func,&data[i]), "pthread_create");
    }

    for(int i = 0; i < n; i++)
        check_pthread(pthread_join(tids[i],NULL), "pthread_join");
    check_pthread(pthread_mutex_destroy(&global_lock), "mutex_destroy");

    printf("%d", global_max);

    exit(EXIT_SUCCESS);
}