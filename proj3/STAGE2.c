/*
CS4352
Project 3 - Beverly G Wertz
STAGE2
*/

//Necessary include files for Stage2
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>

//include for error number retrieval
extern int errno;

//function headers
void visitDir(char* dirName);

//Determines if we have permission to read the directory 
//and then prints out the error or the filename and last 
//modification date
void visitDir(char* dirName)
{
	//Create variables to hold the current entry and directory streams
	DIR *oDir;	
	struct dirent* curEntry;
	struct stat file;
	time_t tm;
	long ltime =1;
	int err = 0;
	
	//Get pathname stat information
	if ((lstat(dirName,&file)) == -1)
	{	
		printf("Error: %s\n",strerror(errno));
		return;
	}
	
	if (!(file.st_mode & S_IEXEC)){
		printf("User does not have execution priveleges to open %s\n",dirName);
		return;
	}
	
	//get the current seconds since Jan. 1, 1978
	time(&ltime);
	
	//Open the directory and create a directory stream
	oDir = opendir(dirName);

	//loop to read the directory while entries are valid
	while ((curEntry = readdir(oDir)) != 0) {
		//Create our new pathname - maxsize of 255 chars
		char *fullPath = malloc(255);
		strcpy(fullPath,dirName); strcat(newPath, "/"); strcat(newPath,curEntry->d_name);
		
		//Check if the filename is valid
		if ((lstat(fullPath,&file)) == -1) continue;
		else { 	
			tm = file.st_mtime;
			printf("File name is %10s last modified %21.20s\n",curEntry->d_name,ctime(&tm)); 
		}
		
		//free allocated memory
		free(fullPath);
	}
	
	//If the read fails, close the directory as there's nothing
	//more to read
	closedir(oDir);
}

//call this program with a directory as an argument
int main(int argc, char* argv[])
{
	//define variables to hold information about the current 
	//directory.
	struct stat buf;
	int err;
	
	//If there are not enough arguments, don't go any further
	//and print a message to the user as to the function
	if (argc < 2) {
		fprintf(stderr,"Usage is:\t%s (directory)\n",argv[0]);
		return(1);
	}
	
	//get information about the supplied argument
	if ((err = (lstat(argv[1],&buf))) == -1)
	{
		printf("Error: %s\n",strerror(errno));
		return;
	}
	
	//Run visitDir function if the argument is a directory
	if (!(S_ISDIR(buf.st_mode))) {
		printf("%s is not a directory\n", argv[1]);
		return(1);
	} else visitDir(argv[1]);

	return(0);
}
