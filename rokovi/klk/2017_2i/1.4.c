#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


#define checkError(cond,msg)\
do{\
	if(!(cond)){\
		perror(msg);\
		exit(EXIT_FAILURE);\
	}\
}while(0)

#define MAX 1024
//Premesti fajl iz src u dest(mv u bashu)
void osMoveFile(const char *srcPath, const char *destPath)
{
	//Ako vec postoji, izadji
	checkError(strcmp(srcPath,destPath) != 0, "isti fajl");

	//Uzimamo podatke o src fajlu
	struct stat fInfo;
	checkError(stat(srcPath, &fInfo) != -1,"stat");
	
	//Otvaramo fajlove za rad
	int fdSrc = open(srcPath, O_RDONLY);
	checkError(fdSrc != -1, "open");
	
	int fdDest = open(destPath, O_WRONLY | O_CREAT | O_TRUNC);
	checkError(fdDest != -1, "open");

	//Kopiramo iz src u dest dok god read procita vise od 0 bajtova
	int bytesRead = 0;
	char memBuff[MAX];
	while((bytesRead = read(fdSrc, memBuff, MAX)) > 0)
	{
		checkError(write(fdDest, memBuff, bytesRead) != -1, "write");
	}	
	checkError(bytesRead != -1, "write");

	close(fdSrc);
	close(fdDest);

	checkError(unlink(srcPath) != -1, "unlink");
	checkError(chmod(destPath, fInfo.st_mode) != -1, "chmod");
}


int main(int argc, char **argv)
{

	checkError(argc == 3,"argv");
	osMoveFile(argv[1],argv[2]);
	return 0;
	
}
