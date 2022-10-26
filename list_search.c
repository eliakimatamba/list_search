#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>


void list_search(int arrayStart, int arrayEnd, int searchKey, int * array, int * fd, int *counter){

    if (array == NULL ) {
    	perror("empty file!");
	    exit(0);
    }

	int status1, status2;
	pid_t leftHalf, rightHalf;

    if(arrayStart == arrayEnd){
    	if(*(array + arrayEnd) == searchKey){
      		printf("%d\n", arrayEnd ); 
    		close(fd[0]);
    		write(fd[1], &counter, sizeof(counter));  	
    	}
    	exit(0);
  	} 

  	else {
  		leftHalf = fork();
	  	if( leftHalf < 0 ){
	    	perror("fork error");
	    	exit(EXIT_FAILURE);
	    }

	    else if (leftHalf == 0){
	     	int leftHalfSize = (int)ceil((arrayStart+arrayEnd)/2); // get the size of left half of array
			list_search(arrayStart, leftHalfSize, searchKey, array, fd, counter); // replace lefthalfsize with arrayStart+arrayEnd)/2 if ceil error
			exit(0);
		} 
		else if (leftHalf > 0) { 
			rightHalf = fork();
			if( rightHalf < 0 ){
	    		perror("fork error");
	    		exit(EXIT_FAILURE);
	    	}
			if (rightHalf == 0) {
			    int rightHalfSize = (int)floor((arrayStart+arrayEnd)/2) + 1;
				list_search(rightHalfSize, arrayEnd, searchKey, array, fd, counter); // replace righthalfsize with (arrayStart+arrayEnd)/2 + 1 if floor error
				exit(0);
		    }
		    else {
		    	waitpid(leftHalf, &status1, 0);
			    waitpid(rightHalf, &status2, 0);
			    if(WEXITSTATUS(status1)==0 || WEXITSTATUS(status2) == 0)
			    	exit(0);
				else{
			   		exit(-1);
				}
			}
		}
	}
}


int main(int argc, char * argv[]){
	char * fileName = argv[1];
	char * key = argv[2];

	char line[80]; // first line of file
	int searchKey = atoi(key); // number to look for in the array
	FILE *fp = fopen(fileName, "r");
	FILE *fp2 = fopen(fileName, "r");
	FILE *fp3 = fopen(fileName, "r");
	int arraySize = 0; // number of elements in file txt

	if (argc != 3){
		perror ("Too (many/few) arguments");
		exit(-1);
	}

	if (fp == NULL) {
		perror ("Error opening file");
		exit(1);
	}

	fgets(line, 20, fp);

	int c;
    for( ;; )
    {
        c = fgetc( fp2 );
        if( c == EOF || c == '\n' )
            break;
        if (c == ' ')
        	++arraySize;
    }
    arraySize++;

    if (arraySize > 10){
    	perror ("Over 10 elements (not allowed)");
		exit(1);
    }

	int array[arraySize]; // file's array to search in

	for (int i = 0; i < arraySize; i++){
		if (fscanf(fp3, "%d ", &array[i]) != 1){
			printf("array file is not in correct format\n");
				return 0;
		}
	}

    fclose(fp);
    fclose(fp2);
    fclose(fp3);

    int fd[2];
	pipe(fd);
	int found[arraySize]; // keeps track of whether indexes were not found
	int counter = 0; // index incrementer for found

    if (fork() == 0)
		list_search(0, arraySize-1, searchKey, array, fd, &counter);
	else{
		wait(NULL);
	}

	close(fd[1]);
    read(fd[0], &counter, sizeof(counter));  	
	if (counter == 0){
		printf("%d\n", -1 );
	}
   	
	return 0;
}

