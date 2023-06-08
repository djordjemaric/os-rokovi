#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <ftw.h>

#define MAX 1024

#define DAY_IN_SEC (24 * 60 * 60)

#define checkError(cond, msg)                    \
	do                                           \
	{                                            \
		if (!(cond))                             \
		{                                        \
			perror(msg);                         \
			printf("%s:%d", __FILE__, __LINE__); \
			exit(EXIT_FAILURE);                  \
		}                                        \
	} while (0)

//Vraca true ukoliko regularni fajl ima rw prava pristupa za others
bool osIsPublicFile(const char *fpath)
{
	struct stat fileInfo;
	checkError(stat(fpath, &fileInfo) != -1, "stat");
	checkError(((fileInfo.st_mode & S_IFMT) == S_IFREG), "tip fajla");
	if ((fileInfo.st_mode & S_IROTH) && (fileInfo.st_mode & S_IWOTH))
		return true;
	else
		return false;
}

unsigned osNumDaysFileModified(const char *fpath)
{
	struct stat fileInfo;
	checkError(stat(fpath, &fileInfo) != -1, "stat");

	time_t now = time(NULL);

	time_t sekundeOdPristupa = fileInfo.st_mtime;
	time_t razlika = now - sekundeOdPristupa;

	return (razlika / DAY_IN_SEC);
}

void osMoveFile(const char *srcPath, const char *destPath)
{

	checkError(strcmp(srcPath, destPath) != 0, "isti fajl");

	struct stat fInfo;
	checkError(stat(srcPath, &fInfo) != -1, "stat");

	int fdSrc = open(srcPath, O_RDONLY);
	checkError(fdSrc != -1, "open");

	int fdDest = open(destPath, O_WRONLY | O_CREAT | O_TRUNC);
	checkError(fdDest != -1, "open");

	int bytesRead = 0;
	char memBuff[MAX];
	while ((bytesRead = read(fdSrc, memBuff, MAX)) > 0)
	{
		checkError(write(fdDest, memBuff, bytesRead) != -1, "write");
	}
	checkError(bytesRead != -1, "write");

	close(fdSrc);
	close(fdDest);

	checkError(unlink(srcPath) != -1, "unlink");
	checkError(chmod(destPath, fInfo.st_mode) != -1, "chmod");
}

void makePublicDir(const char *dname)
{

	mode_t oldUmask = umask(0);
	checkError((mkdir(dname, 0777) == 0), "mkdir");
	umask(oldUmask);
}
char newDir[1024];
int obradi(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	if (S_ISREG(sb->st_mode))
	{
		if (osIsPublicFile(fpath))
		{
			if (osNumDaysFileModified(fpath) < 30)
			{
				char *temp = malloc(strlen(newDir) + strlen(fpath + ftwbuf->base));
				strcpy(temp, newDir);
				strcat(temp, "/");
				strcat(temp, fpath + ftwbuf->base);
				osMoveFile(fpath, temp);
				free(temp);
			}
		}
		else
		{
			unlink(fpath);
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	checkError(argc == 3, "argv");
	struct stat dir;
	checkError(stat(argv[1], &dir) != -1, "ne postoji");
	checkError(S_ISDIR(dir.st_mode), "nije dir");

	strcpy(newDir, argv[2]);
	makePublicDir(argv[2]);
	checkError(nftw(argv[1], obradi, 50, 0) != -1, "nftw");
	exit(EXIT_SUCCESS);
}