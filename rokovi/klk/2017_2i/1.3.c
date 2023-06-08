#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define DAY_IN_SEC (24*60*60)

#define checkError(cond,msg)\
	do{\
		if(!(cond)){\
			perror(msg);\
			exit(EXIT_FAILURE);\
		}\
	}\
	while(0)

//Vrati broj dana od poslednjeg menjanja fajla
unsigned osNumDaysFileModified(const char *fpath)
{
	struct stat fileInfo;
	checkError(stat(fpath, &fileInfo) != -1, "stat");
	
	time_t now = time(NULL);

	time_t sekundeOdPristupa = fileInfo.st_mtime;
	time_t razlika = now - sekundeOdPristupa;

	return (razlika/DAY_IN_SEC);

}


int main(int argc, char** argv)
{
	checkError(argc == 2, "argv");
	printf("%u", osNumDaysFileModified(argv[1]));

	
}
