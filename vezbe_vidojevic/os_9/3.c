#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <sys/wait.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)
    
static const char *os_Usage = "./4 pathToFile";
    
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

int main(int argc, char** argv) {
	
	osAssert(argc == 2, os_Usage);
	
	/* forkujemo novi proces */
	pid_t childPid = fork();
	osAssert(childPid != -1, "fork failed");
	
	/* ako je child proces */
	if (childPid == 0) {
		
		/* execujemo gcc sa argumentom komandne linije */
		osAssert(execlp("gcc", "gcc", argv[1], NULL) != -1, "exec failed");
	}
	
	/* sacekamo da se gcc izvrsi i uhvatimo exit status */
	int status = 0;
	osAssert(waitpid(childPid, &status, 0) != -1, "waitpid failed");
	
	/* izvucemo exit kod */
	if (WIFEXITED(status)) {
		int exitCode = WEXITSTATUS(status);
	
		/* ako nije uspesno zavrseno */
		if (exitCode != EXIT_SUCCESS) {
		
			/* zavrsimo parenta sa istim kodom koji je imao i gcc */
			exit(exitCode);
		}
		
		/* u suprotnom ispisemo velicinu dobijenog izvrsnog fajla */
		struct stat fInfo;
		osAssert(stat("a.out", &fInfo) != -1, "stat failed");
		printf("%jd\n", (intmax_t)fInfo.st_size);
	}
	
	exit(EXIT_SUCCESS);
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
