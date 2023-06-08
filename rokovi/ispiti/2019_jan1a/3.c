#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
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

typedef struct
{
    int id;
} thread_data_t;

int m, n, k;
float **vektori;

void *thread_func(void *arg)
{
    thread_data_t *args = (thread_data_t *)arg;
    float *max = malloc(sizeof(float));
    check_error(max != NULL, "malloc");
    *max = 0;
    for (int i = 0; i < m / k; i++)
    {
        float norma = 0;
        for (int j = 0; j < n; j++)
            norma += vektori[args->id * m / k + i][j] * vektori[args->id * m / k + i][j];
        norma = sqrt(norma);
        if (norma > *max)
            *max = norma;
    }
    return (void *)max;
}

int main(int argc, char **argv)
{
    scanf("%d %d %d", &m, &n, &k);
    vektori = malloc(m * sizeof(float *));
    check_error(vektori != NULL, "malloc");
    for (int i = 0; i < m; i++)
    {
        vektori[i] = malloc(n * sizeof(float));
        check_error(vektori[i] != NULL, "malloc");
        for (int j = 0; j < n; j++)
            scanf("%f", &vektori[i][j]);
    }

    pthread_t *tids = malloc(k * sizeof(pthread_t));
    check_error(tids != NULL, "malloc");

    thread_data_t *data = malloc(k * sizeof(thread_data_t));
    check_error(data != NULL, "malloc");

    for (int i = 0; i < k; i++)
    {
        data[i].id = i;
        check_pthread(pthread_create(&tids[i], NULL, thread_func, (void *)&data[i]), "pthread_create");
    }
    int max_tid;
    float global_max = 0;
    for (int i = 0; i < k; i++)
    {
        float *ret;
        check_pthread(pthread_join(tids[i], (void *)&ret), "pthread_join");

        if (*ret > global_max)
        {
            global_max = *ret;
            max_tid = i;
        }
        // free(ret);
    }

    printf("%d %f\n", max_tid, global_max);

    free(tids);
    free(data);

    for (int i = 0; i < m; i++)
        free(vektori[i]);
    free(vektori);
}