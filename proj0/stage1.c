//Stage 1 code for Beverly G Wertz
//project 0 - Extra Credit
//CS4352 - Spring 2012

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//SIZE is defined as 24 as there are only 24 entries in the student database file
#define SIZE 24

//Define a structure to hold two lines of text to represent the first and second place students
//for the scholarship
typedef struct STUDENT
{
	char FIRST[80];
	char SECOND[80];
} STUDENT;

//function header for the my_print function
void my_print();

int main(int argv, char **argc)
{	
	//Setting up variables for iteration, string and file parsing, file opening and scanning. 
	FILE *fin, *fopen();
	int i;
	char fn[20],ln[20],incI[10],incLine[80],scholStatus;
	float incG,avgGPA,tempGPA;
	
	//I setup a student struct and then a pointer to the struct.
	STUDENT scholarshipStudents,*studentPointer;
	studentPointer = &scholarshipStudents;
	
	//If the file can't be opened, I let the user know and then exit the program.
	if((fin = fopen("/home/courses1/cs3352/projects/proj1/P1-DATA","r"))==NULL) {
		printf("Unable to open file /home/courses1/cs3352/projects/proj1/P1-DATA\n");
		exit(1);
	} 
	
	//I print the header for each column of data to be printed
	fprintf(stdout, "\n%-20s %-20s %9s %4s\n", "First Name","Last Name","ID","GPA");
	
	//Initializing both the iterator of the 24 records and average GPA to zero
	i = 0;
	avgGPA = 0.0;
	//loop until no more lines to read or max number of students read.
	while (1 && i < SIZE) {
		//If we reach the end of the file, exit the loop.
		if (feof(fin)) break;
		//The first character of any line designates the current scholarship status, this time we ignore it
		scholStatus = fgetc(fin);
		//Get the rest of the text and then scan the first and last names, ID number and GPA
		fgets(incLine,80,fin);
		sscanf(incLine,"%s %s %s %f",fn,ln,incI,&incG);
		//Print the line of data just read in pretty print. Also, add the newly read in GPA to the 
		//avgGPA variable and increment i
		fprintf(stdout,"%-20s %-20s %9s %1.2f\n",fn,ln,incI,incG);
		avgGPA+=incG;
		i++;
	}
	//Now we find the average GPA
	avgGPA/=SIZE;
	//Print out the average GPA
	fprintf(stdout,"\nAverage GPA is %1.2f\n",avgGPA);
	
	//We want to reopen the same file to reach in the students again to check for scholarship candidates.
	if((fin = freopen("/home/courses1/cs3352/projects/proj1/P1-DATA","r",fin))==NULL) {
		printf("Unable to reopen file /home/courses1/cs3352/projects/proj1/P1-DATA\n");
		exit(1);
	}
	
	i = 0;
	//Initial section is very similar to original loop, read until end of file or max students
	//Get scholarship status and then read and parse the rest of the line. 
	while (1 && i < SIZE) {
		if (feof(fin)) break;
		scholStatus = fgetc(fin);
		fgets(incLine,80,fin);
		sscanf(incLine,"%s %s %s %f",fn,ln,incI,&incG);
		//We check to see if this is a current scholarship student, if not and their GPA is greater than
		//the average GPA, check to see if there is a student in the first slot, if not, add this student
		//If so, parse the first slot student and compare the two student's GPA.
		if (scholStatus != '#' && incG > avgGPA) {
			if (scholarshipStudents.FIRST == NULL)
				strcpy(scholarshipStudents.FIRST,incLine);
			else {
				sscanf(scholarshipStudents.FIRST,"%s %s %s %f",fn,ln,incI,&tempGPA);
				//If the new student's GPA is higher than the First student's GPA, we copy the first to
				//the second, then copy the new student to the first.
				if (incG > tempGPA) {
					strcpy(scholarshipStudents.SECOND,scholarshipStudents.FIRST);
					strcpy(scholarshipStudents.FIRST,incLine);
				//If the second student is empty, add this student to the second slot.
				} else if (scholarshipStudents.SECOND == NULL)
					strcpy(scholarshipStudents.SECOND,incLine);
				else {
					//Otherwise, compare the students as done for the first student position and replace
					//the second position with the current student, if needed.
					sscanf(scholarshipStudents.SECOND,"%s %s %s %f",fn,ln,incI,&tempGPA);
					if (incG > tempGPA) {
						strcpy(scholarshipStudents.SECOND,incLine);
					}
				}
			}		
		i++;
		}
	}
	
	//Call the function to print the students and then close the file stream.
	my_print(studentPointer);
		
	fclose(fin);

return 0;
}

/*Since this function is taking a struct which contains two student members, it doesn't need to be called
more than once in the defined program as stated in the project notes.*/
void my_print(incStudents)
struct STUDENT *incStudents;
{	//Setup variables to parse then output a string in pretty print.
	char fn[20],ln[20],incI[10];
	float incG;
	//Informing the user of the meaning of the student data about to be printed.
	fprintf(stdout,"\nThe scholarship recipients are:\n");
	//Parse the text and then print the data
	sscanf(incStudents->FIRST,"%s %s %s %f",fn,ln,incI,&incG);
	fprintf(stdout,"%-20s %-20s %9s %1.2f\n",fn,ln,incI,incG);
	//Same for the second student
	sscanf(incStudents->SECOND,"%s %s %s %f",fn,ln,incI,&incG);
	fprintf(stdout,"%-20s %-20s %9s %1.2f\n",fn,ln,incI,incG);
}
