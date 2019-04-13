#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "helper.h"


int get_file_size(char *filename) {
    struct stat sbuf;

    if ((stat(filename, &sbuf)) == -1) {
       perror("stat");
       exit(1);
    }

    return sbuf.st_size;
}

/* A comparison function to use for qsort */
int compare_freq(const void *rec1, const void *rec2) {

    struct rec *r1 = (struct rec *) rec1;
    struct rec *r2 = (struct rec *) rec2;

    if (r1->freq == r2->freq) {
        return 0;
    } else if (r1->freq > r2->freq) {
        return 1;
    } else {
        return -1;
    }
}


// This is a Wrapper function used to error check the Pipe() system call. 
void Pipe(int* fd)
{
    if(pipe(fd) == -1)
    {
        perror("perror");
        exit(1); 
    }
}
// This is a Wrapper function used to error check the Fopen() call 
FILE* Fopen(char* file, char* type)
{
    FILE* outputFile = fopen(file, type);
    if(outputFile == NULL)
    {
        fprintf(stderr, "Error opening the binary file");
	exit(1); 
    }
    return outputFile; 
}

// This is a Wrapper function used to error check the Fork() system call. 
int Fork()
{
    int forkResult = fork(); 
    if(forkResult < 0)
    {
        perror("fork");
        exit(1); 
    }
    return forkResult; 
}

// This is a Wrapper function used to error check the Malloc() system call. 
void *Malloc(int size)
{
    void *ptr; 
    if((ptr = malloc(size)) == NULL)
    {
        perror("malloc"); 
        exit(1); 
    }
    return ptr; 
}


// This is a Wrapper function used to error check the Fclose() call 
void Fclose(FILE* binaryFile)
{
    int error = fclose(binaryFile); 
    if(error != 0)
    {
        perror("fclose");
	exit(1); 
    }
}


// This is a helper function that writes all the records to 
// the output record file 

void writeToOutputFile(struct rec* finalOutput, int recordCount, FILE* outputFile)
{
    for(int b = 0;b<recordCount;b++)
    {
        if(fwrite(&finalOutput[b], sizeof(struct rec), 1, outputFile) != 1)
        {
            fprintf(stderr, "Could not write to output file");
            exit(1); 
        }
        //printf("WRITTEN: %d\n", finalOutput[b].freq);
    }
}

// This is a helper function that finds the minimum element in an 
// array of struct records. Minimum struct rec is the rec with the 
// smallest value for frequency 

void findMin(struct rec* parentRead, int* minIndex, struct rec* min, int processCount){
    for(int p = 0;p<processCount;p++)
    {
        if(parentRead[p].freq != -1 && parentRead[p].freq < (*min).freq)
        {
            *min = parentRead[p]; 
	    *minIndex = p; 
        }
    }
}

// This is a helper function that returns a pointer to an array of integers.  
// The array returned represents the optimal (most even) assignments 
// of records to each process 

int* optimizeSplit(int* numOfProcesses, int numOfRecords)
{

    if(*numOfProcesses <= numOfRecords)
    {
        int* assignments = Malloc(sizeof(int) * (*numOfProcesses));
        for(int j = 0;j<*numOfProcesses;j++)
            assignments[j] = (int)(numOfRecords/ *numOfProcesses);

	int mod = numOfRecords % *numOfProcesses; 
	    for(int i = 0;i<mod;i++)
                assignments[i]++; 
   
        return assignments; 
    }
    else 
    {
        int* assignments = Malloc(sizeof(int) * numOfRecords);
        for(int k = 0;k<numOfRecords;k++)
            assignments[k] = 1; 
        *numOfProcesses = numOfRecords; 

        return assignments; 
    }
   
}




