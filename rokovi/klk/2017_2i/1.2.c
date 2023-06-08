#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#define checkError(cond,msg)\
	do{\
		if(!(cond)){\
			perror(msg);\
			exit(EXIT_FAILURE);\
		}\
	}\
	while(0)

void makePublicDir(const char *dname)
{

	mode_t oldUmask = umask(0);
	checkError((mkdir(dname,0777)==0),"mkdir");
	umask(oldUmask);
}

int main(int argc, char **argv)
{
	checkError(argc == 2, "argc");
	makePublicDir(argv[1]);
	return 0;	
}
