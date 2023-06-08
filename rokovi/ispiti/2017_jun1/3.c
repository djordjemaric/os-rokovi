#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
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
int n, m, k;
int **matrica1;
int **matrica2;
int **rezultat;
int global_max;

typedef struct
{
    int i;
    int j;
} thread_data_t;

void *thread_func(void *arg)
{
    thread_data_t *args = (thread_data_t *)arg;
    int i, j;
    i = args->i;
    j = args->j;
    int element = 0;
    for (int b = 0; b < m; b++)
        element += matrica1[i][b] * matrica2[b][j];
    check_pthread(pthread_mutex_lock(&global_lock), "mutex_lock");
    rezultat[i][j] = element;
    if (element > global_max)
        global_max = element;
    check_pthread(pthread_mutex_unlock(&global_lock), "mutex_unlock");
    return NULL;
}

int main(int argc, char **argv)
{

    // alokacija i ucitavanje matrica
    scanf("%d %d", &n, &m);
    matrica1 = malloc(n * sizeof(int *));
    check_error(matrica1 != NULL, "malloc");
    for (int i = 0; i < n; i++)
    {
        matrica1[i] = malloc(m * sizeof(int));
        check_error(matrica1[i] != NULL, "malloc");
        for (int j = 0; j < m; j++)
        {
            scanf("%d", &matrica1[i][j]);
        }
    }

    scanf("%d %d", &m, &k);
    matrica2 = malloc(m * sizeof(int *));
    check_error(matrica2 != NULL, "malloc");
    for (int i = 0; i < m; i++)
    {
        matrica2[i] = malloc(k * sizeof(int));
        check_error(matrica2[i] != NULL, "malloc");
        for (int j = 0; j < k; j++)
        {
            scanf("%d", &matrica2[i][j]);
        }
    }

    rezultat = malloc(n * sizeof(int *));
    check_error(rezultat != NULL, "malloc");
    for (int i = 0; i < n; i++)
    {
        rezultat[i] = malloc(k * sizeof(int));
        check_error(rezultat[i] != NULL, "malloc");
    }
    //////////
    // Rad sa nitima

    pthread_t *tids = malloc(n * k * sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    thread_data_t *data = malloc(n * k * sizeof(thread_data_t));
    check_error(data != NULL, "malloc");
    check_pthread(pthread_mutex_init(&global_lock, NULL), "mutex_init");

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < k; j++)
        {
            data[i * k + j].i = i;
            data[i * k + j].j = j;
            check_pthread(pthread_create(&tids[i * k + j], NULL, thread_func, (void *)&data[i * k + j]), "pthread_create");
        }
    }

    for (int i = 0; i < n * k; i++)
        check_pthread(pthread_join(tids[i], NULL), "pthread_join");

    check_pthread(pthread_mutex_destroy(&global_lock), "mutex_destroy");

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < k; j++)
            printf("%d ", rezultat[i][j]);
        printf("\n");
    }

    printf("%d", global_max);

    // oslobadjanje memorije
    for (int i = 0; i < n; i++)
        free(matrica1[i]);
    free(matrica1);

    for (int i = 0; i < m; i++)
        free(matrica2[i]);
    free(matrica2);

    for (int i = 0; i < n; i++)
        free(rezultat[i]);
    free(rezultat);

    free(tids);
    free(data);
    exit(EXIT_SUCCESS);
}