//my_print function Stage2 for Beverly G Wertz
//project 0 - Extra Credit
//CS 4352 Spring 2012

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_header.h"

/*Since this function is taking a struct which contains two student members, it doesn't need to be called
more than once in the defined program as stated in the project notes.*/
void my_print(incStudents)
struct STUDENT *incStudents;
{
	char fn[20],ln[20],incI[10];
	float incG;
	
	fprintf(stdout,"\nThe scholarship recipients are:\n");
	
	sscanf(incStudents->FIRST,"%s %s %s %f",fn,ln,incI,&incG);
	fprintf(stdout,"%-20s %-20s %9s %1.2f\n",fn,ln,incI,incG);
	
	sscanf(incStudents->SECOND,"%s %s %s %f",fn,ln,incI,&incG);
	fprintf(stdout,"%-20s %-20s %9s %1.2f\n",fn,ln,incI,incG);
}
