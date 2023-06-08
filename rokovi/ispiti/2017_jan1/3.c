#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include <pthread.h>
#define check_error(cond, msg)                                                                           \
    do                                                                                                   \
    {                                                                                                    \
        if (!(cond))                                                                                     \
        {                                                                                                \
            perror(msg);                                                                                 \
            fprintf(stderr, "File: %s\n Function: %s \n, Line number:%d", __FILE__, __func__, __LINE__); \
            exit(EXIT_FAILURE);                                                                          \
        }                                                                                                \
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

double **matrica;
int n, m;
double globalMin;
pthread_mutex_t globalLock;

typedef struct
{
    int red;
    int id;
} thread_data_t;

void *thread_func(void *arg)
{
    thread_data_t *args = (thread_data_t *)arg;
    int tren_red = args->red;

    double local_min = matrica[tren_red][0];
    for (int i = 1; i < m; i++)
    {
        if (matrica[tren_red][i] < local_min)
            local_min = matrica[tren_red][i];
    }
    check_pthread(pthread_mutex_lock(&globalLock), "mutex_lock");
    if (local_min < globalMin)
        globalMin = local_min;
    check_pthread(pthread_mutex_unlock(&globalLock), "mutex_unlock");

    return NULL;
}

// usage ./a.out path/to/file
int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    FILE *ulaz = fopen(argv[1], "r");
    check_error(ulaz != NULL, "fopen");

    // Ucitavanje matrice
    fscanf(ulaz, "%d %d", &n, &m);
    matrica = malloc(n * sizeof(double *));
    check_error(matrica != NULL, "malloc");
    for (int i = 0; i < n; i++)
    {
        matrica[i] = malloc(m * sizeof(double));
        check_error(matrica[i] != NULL, "malloc");
        for (int j = 0; j < m; j++)
            fscanf(ulaz, "%lf", &matrica[i][j]);
    }

    globalMin = matrica[0][0];
    pthread_t *tids = malloc(n * sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    thread_data_t *data = malloc(n * sizeof(thread_data_t));
    check_error(data != NULL, "malloc");

    check_pthread(pthread_mutex_init(&globalLock, NULL), "mutex_init");

    for (int i = 0; i < n; i++)
    {
        data[i].id = i + 1;
        data[i].red = i;
        check_pthread(pthread_create(&tids[i], NULL, thread_func, &data[i]), "pthread_create");
    }

    for (int i = 0; i < n; i++)
        check_pthread(pthread_join(tids[i], NULL), "pthread_join");

    check_pthread(pthread_mutex_destroy(&globalLock), "mutex_destroy");

    for (int i = 0; i < n; i++)
        free(matrica[i]);
    free(matrica);
    free(data);
    free(tids);
    printf("%lf", globalMin);

    exit(EXIT_SUCCESS);
}