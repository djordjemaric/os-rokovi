#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_LEN (1024)
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

pthread_mutex_t global_lock;
int global_max_reci = 0;
int global_max_indeks;

typedef struct
{
    int id;
    char *path;
} thread_data_t;

typedef struct
{
    int suma;
} thread_return_t;

void *thread_func(void *arg)
{
    thread_data_t *args = (thread_data_t *)arg;
    FILE *f = fopen(args->path, "r");
    check_error(f != NULL, "fopen");

    char rec[MAX_LEN];
    int lok_brojac = 0;
    while (fscanf(f, "%s", rec) != EOF)
        lok_brojac++;
    thread_return_t *ret_val = malloc(sizeof(thread_return_t));
    ret_val->suma = lok_brojac;
    check_pthread(pthread_mutex_lock(&global_lock), "mutex_lock");
    if (lok_brojac > global_max_reci)
    {
        global_max_indeks = args->id;
        global_max_reci = lok_brojac;
    }
    check_pthread(pthread_mutex_unlock(&global_lock), "mutex_unlock");
    fclose(f);
    return ret_val;
}

int main(int argc, char **argv)
{
    check_error(argc > 1, "args");

    int n = argc - 1;

    pthread_t *tids = malloc(n * sizeof(pthread_t));
    check_error(tids != NULL, "malloc");

    thread_data_t *data = malloc(n * sizeof(thread_data_t));
    check_error(data != NULL, "malloc");

    check_pthread(pthread_mutex_init(&global_lock, NULL), "mutex_init");

    for (int i = 0; i < n; i++)
    {
        data[i].id = i;
        data[i].path = argv[i + 1];

        check_pthread(pthread_create(&tids[i], NULL, thread_func, (void *)&data[i]), "pthread_create");
    }
    int ukupno_reci = 0;
    for (int i = 0; i < n; i++)
    {
        thread_return_t *ret_val;
        check_pthread(pthread_join(tids[i], (void **)&ret_val), "pthread_join");
        ukupno_reci += ret_val->suma;
        free(ret_val);
    }
    printf("%d %d %s", ukupno_reci, global_max_reci, argv[global_max_indeks + 1]);
    check_pthread(pthread_mutex_destroy(&global_lock), "mutex_destroy");
    free(tids);
    free(data);
    exit(EXIT_SUCCESS);
}