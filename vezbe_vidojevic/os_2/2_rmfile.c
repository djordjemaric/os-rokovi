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

int main(int argc, char **argv) {
    osAssert(3 == argc, "Argument missing. Usage: ./rmfile -[fd] path/to/file/or/directory");
    
    osAssert('-' == argv[1][0], "Not a command line option");
    if ('f' == argv[1][1])
		/* brisanje regularnog fajla
		 * TLPI za detalje 
		 */
      osAssert(unlink(argv[2]) >= 0, "Removing file failed");
    else if ('d' == argv[1][1])
		/* brisanje direktorijuma
		 * TLPI za detalje 
		 */
      osAssert(rmdir(argv[2]) >= 0, "Removing directory failed");
    else 
      osAssert(false, "Wrong option, valid are 'f' and 'd'");
    return 0;
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
