#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
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

#define ARRAY_MAX (1024)

typedef struct
{
    sem_t inDataReady;
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsData;

void *get_mem_block(char *path, unsigned *size)
{
    int mem_fd = shm_open(path, O_RDWR, 0644);
    check_error(mem_fd != -1, "shm_open");

    struct stat f_info;
    check_error(fstat(mem_fd, &f_info) != -1, "fstat");
    *size = f_info.st_size;

    void *addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    check_error(addr != MAP_FAILED, "mmap");
    close(mem_fd);
    return addr;
}

int ima_jedinice(int x)
{
    int jedinica = 0;
    unsigned maska = 1 << 31;
    int i = 0;
    while (maska)
    {
        if (x & maska)
            jedinica++;
        maska >>= 1;
        i++;
    }
    return jedinica >= 4;
}

int main(int argc, char **argv)
{
    check_error(argc == 3, "args");

    unsigned size_u = 0;
    OsData *ulazna = get_mem_block(argv[1], &size_u);

    unsigned size_i = 0;
    OsData *izlazna = get_mem_block(argv[2], &size_i);

    check_error(sem_wait(&ulazna->inDataReady) != -1, "sem_wait");
    
    int br_prosledjenih = 0;
    for (int i = 0; i < ulazna->arrayLen; i++)
    {
        if (ima_jedinice(ulazna->array[i]))
        {
            izlazna->array[br_prosledjenih] = ulazna->array[i];
            br_prosledjenih++;
        }
    }
    izlazna->arrayLen = br_prosledjenih;

    check_error(sem_post(&izlazna->inDataReady) != -1, "sem_post");

    check_error(munmap(ulazna, size_u) != -1, "munmap");
    check_error(munmap(izlazna, size_i) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
