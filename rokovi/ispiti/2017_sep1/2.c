#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

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

#define check_pthread(error, msg)    \
    do                               \
    {                                \
        int _err = error;            \
        if (_err > 0)                \
        {                            \
            errno = _err;            \
            check_error(false, msg); \
        }                            \
    } while (0)

int v;
pthread_mutex_t global_lock;
int global_max = 0;
int global_max_id;
int **matrica;

typedef struct
{
    int kolona;
    int stepen;
    int id;
} thread_data_t;

void *thread_func(void *arg)
{
    thread_data_t *args = (thread_data_t *)arg;
    int stepen = 0;
    for (int i = 0; i < v; i++)
        stepen += matrica[i][args->kolona];
    pthread_mutex_lock(&global_lock);
    if (stepen > global_max)
    {
        global_max = stepen;
        global_max_id = args->id;
    }
    pthread_mutex_unlock(&global_lock);
    return NULL;
}
int main(int argc, char **argv)
{
    scanf("%d", &v);
    matrica = malloc(v * sizeof(int *));
    check_error(matrica != NULL, "malloc");
    for (int i = 0; i < v; i++)
    {
        matrica[i] = malloc(v * sizeof(int));
        check_error(matrica[i] != NULL, "malloc");
        for (int j = 0; j < v; j++)
            scanf("%d", &matrica[i][j]);
    }

    pthread_t *tids = malloc(v * sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    thread_data_t *data = malloc(v * sizeof(thread_data_t));
    check_error(data != NULL, "malloc");

    check_pthread(pthread_mutex_init(&global_lock, NULL), "mutex_init");

    for (int i = 0; i < v; i++)
    {
        data[i].id = i;
        data[i].kolona = i;

        check_pthread(pthread_create(&tids[i], NULL, thread_func, &data[i]), "pthread_create");
    }

    for (int i = 0; i < v; i++)
        check_pthread(pthread_join(tids[i], NULL), "pthread_join");
    check_pthread(pthread_mutex_destroy(&global_lock), "mutex_destroy");

    printf("%d", global_max_id);

    // oslobadjanje
    for (int i = 0; i < v; i++)
        free(matrica[i]);
    free(matrica);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}