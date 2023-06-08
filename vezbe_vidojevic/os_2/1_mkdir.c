#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

/* funkcija kreira direktorijum na prosledjenoj putanji */
bool osCreateDirectory(const char *dirPath);

int main(int argc, char **argv) {
    osAssert(2 <= argc, "Argument missing");
    osAssert(osCreateDirectory(argv[1]), "Create directory failed");
    return 0;
}
/* funkcija kreira direktorijum na prosledjenoj putanji */
bool osCreateDirectory(const char *dirPath) {
  /* privillegije sa kojima se kreira direktorijum */
  static mode_t mode = 0755;
  /* kreiranje direktorijuma
   * TLPI za detalje
   */
  return mkdir(dirPath, mode) >= 0;
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
