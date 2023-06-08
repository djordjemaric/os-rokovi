/* exec je sistemski poziv koji omogucava izvrsavanje drugih programa. 
Fork vrsi kopiranje adresnog prostora, exec vrsi zamenu adresnog prostora
Nakon exec-a sadrzaj adresnog prostora se prepisuje novim procesom.
*/ 
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/wait.h>
#include <string.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
	do { \
		if (!(expr)) \
			osErrorFatal(userMsg); \
	} while (0)
	
#define READ_END 	(0)
#define WRITE_END 	(1)
#define BUF_SIZE 	(256)	
	
void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNumber);

int main(int argc, char** argv) {
	
	osAssert(execlp("ls", "ls", "-l", NULL) != -1, "exec failed");
	
	exit(EXIT_SUCCESS);
}

void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNumber) {
	
	perror(userMsg);
	
	fprintf(stderr, "File name: %s\nFunction name: %s\nLine number: %d\n", fileName, functionName, lineNumber);
	
	exit(EXIT_FAILURE);
}
