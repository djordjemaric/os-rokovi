#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#define ARRAY_MAX 1024

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\

typedef struct{
    sem_t inDataReady;
    float array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

void *create_mem_block(char *path, unsigned size){
    int mem_fd = shm_open(path, O_CREAT | O_RDWR,0644);
    check_error(mem_fd != -1, "shm_open");

    check_error(-1 != ftruncate(mem_fd,size), "ftruncate");

    void *addr;
    check_error(MAP_FAILED != (addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd,0)), "mmap");
    
    close(mem_fd);
    return addr;
}

int main(int argc, char **argv){
    unsigned size = sizeof(OsInputData);
    OsInputData *mem_block = create_mem_block(argv[1], size);
    check_error(sem_init(&mem_block->inDataReady,1,0) != -1, "sem_init");
    mem_block->arrayLen = 2;
    mem_block->array[0] = 1.0;
    mem_block->array[1] = 4.0;

    check_error(sem_post(&mem_block->inDataReady) != -1, "sem_post");
    check_error(munmap(mem_block, size) != -1, "munmap");
    exit(EXIT_SUCCESS);
}