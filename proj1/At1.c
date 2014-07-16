//Beverly G Wertz
//CS4352 - Project 1
//At command to extract and run a command at a specified time by the user
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

//Initialize the global variables that we need to have access to througout the 
//program
char **arglist, *cstime;

//Process arguments which extracts the commands from the command prompt and 
//then displays them to the user
void process_arguments(int argc, char **argv)
{
	//Allocating an array of c-strings which is the length of the supplied
	//arguments
	arglist = (char**) malloc(argc*sizeof(char *));

	//Allocate enough space to hold the first argument beyond the executable
	//name then copy it into the string followed by a print of the string
	cstime = (char *) malloc(strlen(argv[1])*sizeof(char));
	strcpy(cstime, argv[1]);		
	printf("Time = %s\n",cstime);
	
	//For each additional argument beyond the time provided, first allocate
	//enough space for the string, then copy the string, followed by a display
	//of the string. If this is not the last argument, display a comma and space.
	//If it is the last argument, display a period then newline.
	int i;
	for (i=0;i + 2 < argc; i++) {
		arglist[i] = (char *) malloc(strlen(argv[i+2])*sizeof(char));
		strcpy(arglist[i],argv[i+2]);
		printf("arglist[%d] = \"%s\"",i,arglist[i]);
		if (i + 2 < argc-1)
			printf(", ");
		else
			printf(".\n");
	}
}

int get_time_compute_delay(char * cstime) 
{
	char schr[3], scmin[3], scsec[3], sihr[3], simin[3], sisec[3], timerType;
	int totalSeconds = 0, secSinceMid = 0, chr, cmin, csec, ihr, imin, isec, i;
	
	//Test if timer type is delay or absolute time, if no valid character is 
	//entered, print error and exit.
	if (cstime[0] == '@')
		timerType = 'a';
	else if (cstime[0] == '+')
		timerType = 'd';
	else {
		printf("Error - time argument should begin with a '+' or '@' only\n");	
		return -1;
	}
	
	//compute time since midnight for absolute time
	if (timerType == 'a')
	{
		FILE *dt, *popen();
		if ((dt = popen("/bin/date '+%H %M %S'", "r")) <0)
			printf("popen failed \n");
		fscanf(dt, "%s %s %s", schr, scmin, scsec);
		chr = atoi(schr); cmin = atoi(scmin); csec = atoi(scsec);
		pclose(dt);
		secSinceMid = 60*60*chr+60*cmin+csec;
	}
	
	//extract digits from user supplied string which should be in the format of 00:00:00
	sihr[2] = simin[2] = sisec[2] = '\0';
	int len = strlen(cstime);
	
	//Extract Hours - and print errors if the values are invalid
	if (len > 2) {
		sihr[0] = cstime[1]; sihr[1] = cstime[2];
		if (!isNum(sihr[0]) || !isNum(sihr[1])) {
			printf("Error - hour must be two digits.\n");
			return -1;
		}
	}
	else {
		printf("Error - hour must be two digits.\n");
		return -1;
	}
	
	//Extract Minutes & print errors if the values are invalid
	if (len > 5) {
		simin[0] = cstime[4]; simin[1] = cstime[5];
		if (!isNum(simin[0]) || !isNum(simin[1])) {
			printf("Error - minutes must be two digits.\n");
			return -1;
		}
	}
	else {
		printf("Error - minutes must be two digits.\n");
		return -1;
	}
	
	//Extract Seconds - Print errors if the values are invalid
	if (len > 8) {
		sisec[0] = cstime[7]; sisec[1] = cstime[8];
		if (!isNum(sisec[0]) || !isNum(sisec[1])) {
			printf("Error - seconds must be two digits.\n");
			return -1;
		}
	}
	else {
		printf("Error - seconds must be two digits.\n");
		return -1;
	}
	
	//Convert digits to numbers and then check to ensure they don't exceed max
	//values
	ihr = atoi(sihr); imin = atoi(simin); isec = atoi(sisec);
	if (ihr > 23) {
		printf("Error - hours exceed 23\n");
		return -1;
	} else if (imin > 59) {
		printf("Error - minutes exceed 59\n");
		return -1;
	} else if (isec > 59) {
		printf("Error - seconds exceed 59\n");
		return -1;
	}
	
	//Now calculate the time delay until the process should execute
	totalSeconds = 60*60*ihr+60*imin+isec;
	if (timerType == 'a') {
		if (totalSeconds > secSinceMid)
			totalSeconds-=secSinceMid;
		else
			totalSeconds+=(60*60*24-secSinceMid);
	}
	
	return totalSeconds;
}

//Function to test if character c is a digit 
int isNum(char dig) { 
	if( dig >= '0' && dig <= '9' ) 
		return 1; 

	return 0; 
}

int main(int argc, char **argv)
{
	//If no command line arguments supplied, then we should exit the program
	if (argc < 2) {
		printf("Command line arguments must be supplied to run this program\n");
		return 1;
	}
	//Pass the command line arguments to the function
	process_arguments(argc,argv);
	
	//Get the delay in seconds. 
	int delay = get_time_compute_delay(cstime);
	if (delay < 0 )
		return 1;	
	printf("Time delay is : %d\n",delay);
	
	//Determine if the command exists on the Path, exit if there is no program 
	//or no valid command
	if (argc < 3) {
		printf("No program submitted to run, exiting program\n");
		return 1;
	} else {
		char *path = pathfind(getenv("PATH"),arglist[0],"rx");
		if (path != NULL)
			printf("Path found for command %s: %s\n",arglist[0],path);
		else {
			printf("No path found for command %s\n",arglist[0]);
			return 1;
		}
	}
	
	//Sleep for delay and then run date command
	sleep(delay);
	system("date");
	
	return 0;
}

