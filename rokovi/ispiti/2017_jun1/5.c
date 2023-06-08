#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
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

#define ARRAY_MAX 1024
typedef struct
{
    sem_t inDataReady;
    float array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

void *get_memory_block(char *path, unsigned *size)
{

    int mem_fd = shm_open(path, O_RDWR, 0777);
    check_error(mem_fd != -1, "shm_open");
    struct stat file_info;
    check_error(fstat(mem_fd, &file_info) != -1, "stat");
    *size = file_info.st_size;

    void *address = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    check_error(address != NULL, "mmap");
    close(mem_fd);
    return address;
}

int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    unsigned size = 0;
    OsInputData *data = get_memory_block(argv[1], &size);

    check_error(sem_wait(&(data->inDataReady)) != -1, "sem_wait");
    float ar_sredina = 0;
    for (int i = 0; i < data->arrayLen; i++)
        ar_sredina += data->array[i];
    ar_sredina /= (float)data->arrayLen;

    float suma = 0;
    for (int i = 0; i < data->arrayLen; i++)
    {
        float razlika = data->array[i] - ar_sredina;
        suma += razlika * razlika;
    }
    float st_dev = suma / (float)data->arrayLen;
    st_dev = sqrt(st_dev);
    printf("%f", st_dev);

    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}