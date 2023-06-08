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

void *make_mem_block(char *path, unsigned size)
{
    int mem_fd = shm_open(path, O_CREAT | O_RDWR, 0644);
    check_error(mem_fd != -1, "shm_open");
    ftruncate(mem_fd, size);

    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    check_error(addr != MAP_FAILED, "mmap");
    close(mem_fd);
    return addr;
}

int main(int argc, char **argv)
{
    check_error(argc > 1, "args");
    unsigned size = sizeof(OsData);
    OsData *mem_block = make_mem_block(argv[1], size);
    int n = argc - 2;
    check_error(sem_init(&mem_block->inDataReady, 1, 0) != -1, "sem_init");
    mem_block->arrayLen = 0;
    for (int i = 0; i < n; i++)
    {
        mem_block->array[i] = atoi(argv[i + 2]);
        mem_block->arrayLen++;
    }
    if (mem_block->arrayLen)
        check_error(sem_post(&mem_block->inDataReady) != -1, "sem_post");

    check_error(munmap(mem_block, size) != -1, "munmap");
    exit(EXIT_SUCCESS);
}