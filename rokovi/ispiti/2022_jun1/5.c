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

void *get_mem_block(char *path, unsigned*size){
    int mem_fd = shm_open(path,O_RDWR,0644);
    check_error(mem_fd != -1, "shm_open");

    struct stat file_info;
    check_error(-1 != fstat(mem_fd, &file_info), "fstat");
    *size = file_info.st_size;

    void *addr = NULL;

    check_error((addr = mmap(NULL, *size, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd,0)) != MAP_FAILED, "mmap");
    close(mem_fd);
    return addr;
}

int main(int argc, char **argv){
    check_error(argc == 2, "args");
    int size = 0;
    OsInputData *data = get_mem_block(argv[1], &size);
    check_error(sem_wait(&data->inDataReady) != -1, "sem_wait");
    float suma = 0;
    for(int i = 0; i < data->arrayLen; i++){
        suma += data->array[i];
    }
    suma = suma / (float)data->arrayLen;

    printf("%f", suma);

    check_error(munmap(data,size) != -1, "munmap");
    exit(EXIT_SUCCESS);

}