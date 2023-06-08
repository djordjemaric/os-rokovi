#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
/*
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
*/

#define ARRAY_MAX 1024
typedef struct
{
    sem_t inDataReady;
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

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

void *get_memory_block(char *path, int *size)
{
    int mem_fd = shm_open(path, O_RDWR, 0777);
    check_error(mem_fd != -1, "shm_open");
    struct stat file_info;
    check_error(fstat(mem_fd, &file_info) != -1, "fstat");
    *size = file_info.st_size;
    void *address = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    check_error(address != MAP_FAILED, "mmap");
    close(mem_fd);
    return address;
}
int prebroj_jedinice(int x)
{
    unsigned maska = 1;
    int brojac = 0;
    while (maska)
    {
        if (x & maska)
            brojac++;
        maska <<= 1;
    }
    return brojac >= 4;
}

int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    int size = 0;
    OsInputData *data = get_memory_block(argv[1], &size);

    check_error(sem_wait(&(data->inDataReady)) != -1, "sem_wait");

    for (int i = 0; i < data->arrayLen; i++)
    {
        if (prebroj_jedinice(data->array[i]))
            printf("%d ", data->array[i]);
    }

    check_error(munmap(data, size) != -1, "munmap");
    exit(EXIT_SUCCESS);
}