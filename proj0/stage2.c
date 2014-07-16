//main program for Stage2 for Beverly G Wertz
//project 0 - Extra Credit
//CS 4352 Spring 2012

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_header.h"
//SIZE is defined as 24 as there are only 24 entries in the student database file
#define SIZE 24

void my_print();

int main(int argv, char **argc)
{
	FILE *fin, *fopen();
	int i;
	char fn[20],ln[20],incI[10],incLine[80],scholStatus;
	float incG,avgGPA,tempGPA;
	
	STUDENT scholarshipStudents,*studentPointer;
	studentPointer = &scholarshipStudents;
	
	if((fin = fopen("/home/courses1/cs3352/projects/proj1/P1-DATA","r"))==NULL) {
		printf("Unable to open file /home/courses1/cs3352/projects/proj1/P1-DATA\n");
		exit(1);
	} 
	
	fprintf(stdout, "\n%-20s %-20s %9s %4s\n", "First Name","Last Name","ID","GPA");
	i = 0;
	avgGPA = 0.0;
	while (1 && i < SIZE) {
		if (feof(fin)) break;
		scholStatus = fgetc(fin);
		fgets(incLine,80,fin);
		sscanf(incLine,"%s %s %s %f",fn,ln,incI,&incG);
		fprintf(stdout,"%-20s %-20s %9s %1.2f\n",fn,ln,incI,incG);
		avgGPA+=incG;
		i++;
	}
	avgGPA/=SIZE;
		
	fprintf(stdout,"\nAverage GPA is %1.2f\n",avgGPA);
	
	if((fin = freopen("/home/courses1/cs3352/projects/proj1/P1-DATA","r",fin))==NULL) {
		printf("Unable to reopen file /home/courses1/cs3352/projects/proj1/P1-DATA\n");
		exit(1);
	}
	
	i = 0;
	while (1 && i < SIZE) {
		if (feof(fin)) break;
		scholStatus = fgetc(fin);
		fgets(incLine,80,fin);
		sscanf(incLine,"%s %s %s %f",fn,ln,incI,&incG);
		if (scholStatus != '#' && incG > avgGPA) {
			if (scholarshipStudents.FIRST == NULL)
				strcpy(scholarshipStudents.FIRST,incLine);
			else {
				sscanf(scholarshipStudents.FIRST,"%s %s %s %f",fn,ln,incI,&tempGPA);
				if (incG > tempGPA) {
					strcpy(scholarshipStudents.SECOND,scholarshipStudents.FIRST);
					strcpy(scholarshipStudents.FIRST,incLine);
				} else if (scholarshipStudents.SECOND == NULL)
					strcpy(scholarshipStudents.SECOND,incLine);
				else {
					sscanf(scholarshipStudents.SECOND,"%s %s %s %f",fn,ln,incI,&tempGPA);
					if (incG > tempGPA) {
						strcpy(scholarshipStudents.SECOND,incLine);
					}
				}
			}		
		i++;
		}
	}
	
	my_print(studentPointer);
		
	fclose(fin);

return 0;
}
