#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_LEN 256

#define checkError(cond, msg)                             \
    do                                                    \
    {                                                     \
        if (!(cond))                                      \
        {                                                 \
            perror(msg);                                  \
            fprintf(stderr, "%s:%d", __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                           \
        }                                                 \
    } while (0)

int main(int argc, char **argv)
{
    checkError(argc == 3, "args");
    int fdSrc, fdDest;
    checkError((fdSrc = open(argv[1], O_RDONLY)) != -1, "open");
    checkError((fdDest = open(argv[2], O_WRONLY)) != -1, "open");

    int offset;
    char *rec = malloc(sizeof(char) * MAX_LEN);
    checkError(rec != NULL, "malloc");
    FILE *src = fdopen(fdSrc, "r");

    while (fscanf(src, "%d %s", &offset, rec) == 2)
    {
        checkError(lseek(fdDest, offset, SEEK_SET) != -1, "lseek");
        int bytesRead, wordLength = strlen(rec);
        checkError((bytesRead = write(fdDest, rec, wordLength)) != -1, "write");
    }
    free(rec);
    close(fdDest);
    fclose(src);
    close(fdSrc);
    exit(EXIT_SUCCESS);
}