/*
CS4352
Project 3 - Beverly G Wertz
STAGE4
*/

//Necessary include files for Stage4
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <pwd.h>
#include <limits.h>

//include for error number retrieval
extern int errno;

//global array to compare possible user passed arguments with
char selTable[4][6] = {"-name","-size","-user",NULL};

//globals to store selection # and selection argument
int selNum, maxSize = 0, uid;
char *matchName;

//function headers
void visitDir(char* dirName);
void printUsage();
int procArg(char* source, char* sel, char* arg);

//Determines if we have permission to read the directory 
//and then prints out the error or the filename and last 
//modification date
void visitDir(char* dirName)
{
	//Create variables to hold the current entry and directory streams
	DIR *oDir;	
	struct dirent* curEntry;
	struct stat file;
	int err = 0, match = 0;
	
	//Get pathname stat information
	if ((lstat(dirName,&file)) == -1) {	
		//perror("Unable to open file");
		return;
	}
	
	//Open the directory and create a directory stream
	if ((oDir = opendir(dirName)) == NULL) {
		//perror(dirName);
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
				//Check name and UID conditions - size can only be checked for regular files
				switch (selNum) {
					case 0: if (strcasecmp(curEntry->d_name,matchName) == 0) match = 1; break;
					case 2: if (file.st_uid == uid) match = 1; break;
					default: break;
				}
			
				//If the file matches the condition, print the line, otherwise don't print
				//If it's a directory, we still will traverse it
				//For regular files, we check if the size is below the user's threshold
				//via short-circuit evaluation
				if (S_ISREG(file.st_mode) &&( match != 0 || (selNum == 1 && file.st_size <= maxSize))) 
					printf("\tREG:\t%s\n",fullPath);
				else if (S_ISDIR(file.st_mode)) {
					if (match != 0) printf("\tDIR:\t%s\n",fullPath);
					visitDir(fullPath);
				} else if (match != 0) printf("-\tOTH:\t%s\n",fullPath);
			}
		}
		
		//free allocated memory
		free(fullPath);
		match = 0;
	}
	
	//If the read fails, close the directory as there's nothing
	//more to read
	if (oDir != NULL)
		closedir(oDir);
}

//Function that determines which selection and arguments were passed by
//the user and stores them in global variables for later use in matching
int procArg(char* source, char* sel, char* arg)
{
	//print out the user supplied variables
	printf("Source: %s\tSelection: %s\tArg: %s\n\n",source, sel, arg);
	
	//setup int value for cstring iteration
	int i = 0;
	struct passwd *user;
	
	//Check argument one for a match, if no match, print an error and exit
	//It also will check the argument supplied and inform the user if it
	//is not valid
	if(strcmp(sel,selTable[0]) == 0) {
		selNum = 0;
		for (i = 0; i < strlen(arg); i++){
			if (isgraph(arg[i]) == 0) {
				printf("Argument for -name must be printable characters.\n");
				return(-1);
			}
		}
		matchName = malloc(strlen(arg));
		strcpy(matchName,arg);
	} else if(strcmp(sel,selTable[1]) == 0) {
		selNum = 1;
		for (i = 0; i < strlen(arg); i++) {
			if (isdigit(arg[i]) == 0) {
				printf("Argument for -size must be numeric.\n");
				return(-1);
			}
		}
		maxSize = atoi(arg);
	} else if(strcmp(sel,selTable[2]) == 0) {
		selNum = 2;
		//check if alphanumeric, if not, show error, if so, malloc the space and save
		for (i = 0; i < strlen(arg); i++){
			if (isgraph(arg[i]) == 0) {
				printf("Argument for -user must be printable characters.\n");
				return(-1);
			}
		}
		matchName = malloc(strlen(arg));
		strcpy(matchName,arg);
		
		//Find a user and get uid if they exist, else print error
		if((user = getpwnam(matchName)) == NULL) {
			printf("User: %s not found\n",matchName);
			return(-1);
		} else
			uid = user->pw_uid;
	} else {
		printf("Selection not valid, valid selections are: -name, -size, -user\n");
		return -1;
	}
	
	return 1;
}

//function to print the usage of the Find function when the user asks for help
void printUsage() {
	printf("Usage is:\tFind (directory) -(selection) (argument)\n");
	printf("Selections: -name, -size, -user\n");
	printf("Arguments for name: filename\n");
	printf("Arguments for size: bytes (finds files smaller than argument)\n");
	printf("Arguments for user: valid userid\n");
	printf("Example:\tFind ~/projects -size 300\n");
	printf("Finds all files in the user's projects folder less than or equal to 300 bytes\n");
}
	

//call this program with a directory as an argument
int main(int argc, char* argv[])
{
	//define variables to hold information about the current 
	//directory.
	struct stat buf;
	int err, i, match;
	char *longPath;
	
	//If there are not enough arguments, don't go any further
	//and print a message to the user as to the function
	if (argc < 4) {
		printf("Usage is:\t%s (directory) -(selection) (argument)\n",argv[0]);
		return(1);
	}
		
	//Process arguments
	if (procArg(argv[1],argv[2],argv[3]) == -1)
		return(1);
		
	//get information about the supplied argument
	if ((err = (lstat(argv[1],&buf))) == -1)
	{
		perror(argv[1]);
		return(1);
	}
	
	//Get the absolute path of the supplied argument
	longPath = malloc(PATH_MAX);
	realpath(argv[1],longPath);
	
	//Run visitDir function if the argument is a directory
	if (!(S_ISDIR(buf.st_mode))) {
		printf("%s is not a directory.\n",longPath);
		return(1);
	} else {
		switch (selNum) {
			case 0: if (strcasecmp(argv[1],matchName) == 0) match = 1; break;
			case 2: if (buf.st_uid == uid) match = 1; break;
			default: break;
		}
		if (match == 1) printf("\tDIR:\t%s\n",longPath);
		visitDir(longPath);
	}

	return(0);
}
