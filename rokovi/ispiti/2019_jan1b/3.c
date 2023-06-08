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

int m, n, k;
float **vektori;

typedef struct
{
    int id;
} thread_data_t;

typedef struct
{
    float sp;
} thread_return_t;

void *thread_func(void *arg)
{
    thread_data_t *args = (thread_data_t *)arg;
    float max_sp = 0;
    float tren_sp;
    for (int i = 0; i < m / k; i++)
    {
        tren_sp = 0;
        for (int j = 0; j < n; j++)
            tren_sp += vektori[args->id * m / k + i][j] * vektori[m][j];
        if (tren_sp > max_sp)
            max_sp = tren_sp;
    }
    thread_return_t *ret_val = malloc(sizeof(thread_return_t));
    check_error(ret_val != NULL, "malloc");
    ret_val->sp = max_sp;
    return (void *)ret_val;
}

int main(int argc, char **argv)
{
    scanf("%d %d %d", &m, &n, &k);
    vektori = malloc((m + 1) * sizeof(float *));
    check_error(vektori != NULL, "malloc");
    for (int i = 0; i < m + 1; i++)
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
    int max_indeks;
    float max_sp = 0;
    for (int i = 0; i < k; i++)
    {
        thread_return_t *ret_val;
        check_pthread(pthread_join(tids[i], (void *)&ret_val), "pthread_join");
        if (ret_val->sp > max_sp)
        {
            max_indeks = i + 1; // traze indeksiranje niti od 1, lakse samo ispisati ovako
            max_sp = ret_val->sp;
        }
        free(ret_val);
    }

    printf("%d %f", max_indeks, max_sp);
    free(tids);
    free(data);
    for (int i = 0; i < m + 1; i++)
        free(vektori[i]);
    free(vektori);
    exit(EXIT_SUCCESS);
}
