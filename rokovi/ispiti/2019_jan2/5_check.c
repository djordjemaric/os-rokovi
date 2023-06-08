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

int main(int argc, char **argv)
{
    check_error(argc == 2, "args");
    unsigned size = 0;
    OsData *memorija = get_mem_block(argv[1], &size);
    sem_wait(&memorija->inDataReady);
    for (int i = 0; i < memorija->arrayLen; i++)
        printf("%d ", memorija->array[i]);
    munmap(memorija, size);
    exit(EXIT_SUCCESS);
}