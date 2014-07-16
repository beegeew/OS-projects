/*
CS4352
Project 3 - Beverly G Wertz
STAGE3
*/

//Necessary include files for Stage3
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
	int err = 0;
	
	//Get pathname stat information
	if ((lstat(dirName,&file)) == -1)
	{	
		perror("Unable to open file");
		return;
	}
	
	//Open the directory and create a directory stream
	if ((oDir = opendir(dirName)) == NULL) {
		perror(dirName);
		return;
	}
	

	//loop to read the directory while entries are valid
	while ((curEntry = readdir(oDir)) != 0) {
		
		//Create our new pathname - maxsize of 255 chars
		char *fullPath = malloc(255);
		strcpy(fullPath,dirName); strcat(fullPath, "/"); strcat(fullPath,curEntry->d_name);
		
		//Check if the filename is valid
		if ((lstat(fullPath,&file)) == -1) continue;
		else {
			//Ensure that the file is not . or ..
			if (strcmp(curEntry->d_name,".") == 0 || strcmp(curEntry->d_name,"..") == 0) continue;
			else {
				if (S_ISREG(file.st_mode)) printf("\tREG:\t%s\n",curEntry->d_name);
				else if (S_ISDIR(file.st_mode)) {
					printf("\tDIR:\t%s\n",curEntry->d_name);
					visitDir(fullPath);
				} else printf("-\tOTH:\t%s\n",curEntry->d_name);
			}
		}
		
		//free allocated memory
		free(fullPath);
	}
	
	//If the read fails, close the directory as there's nothing
	//more to read
	if (oDir != NULL)
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
		printf("Usage is:\t%s (directory)\n",argv[0]);
		return(1);
	}
	
	printf("Source Argument:\t%s\n\n",argv[1]);
	
	//get information about the supplied argument
	if ((err = (lstat(argv[1],&buf))) == -1)
	{
		perror(argv[1]);
		return;
	}
	
	//Run visitDir function if the argument is a directory
	if (!(S_ISDIR(buf.st_mode))) {
		printf("%s is not a directory.\n",argv[1]);
		return(1);
	} else {
		printf("\tDIR:\t%s\n",argv[1]);
		visitDir(argv[1]);
	}

	return(0);
}
