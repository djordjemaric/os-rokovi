#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
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

#define ARRAY_MAX 1024
typedef struct
{
    sem_t inDataReady;
    float array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

void *get_mem_block(char *fpath, int *size)
{
    int mem_fd = shm_open(fpath, O_RDWR, 0777);
    check_error(mem_fd != -1, "shm_open");

    struct stat file_info;
    check_error(fstat(mem_fd, &file_info) != -1, "fstat");
    *size = file_info.st_size;
    void *address = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    check_error(address != MAP_FAILED, "mmap");
    close(mem_fd);
    return address;
}

// usage ./a.out path/to/shm
int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    int size = 0;

    OsInputData *data = get_mem_block(argv[1], &size);

    check_error(sem_wait(&(data->inDataReady)) != -1, "sem_wait");

    for (int i = 0; i < data->arrayLen; i++)
    {
        for (int j = i + 1; j < data->arrayLen; j++)
        {
            if (data->array[j] < data->array[i])
            {
                float pom = data->array[i];
                data->array[i] = data->array[j];
                data->array[j] = pom;
            }
        }
    }

    printf("%f", data->array[(data->arrayLen) / 2]);
    check_error(munmap(data, size) != -1, "munmap");
    exit(EXIT_SUCCESS);
}