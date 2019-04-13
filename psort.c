 

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "helper.h"
#include <sys/wait.h>


// The Main function begins here 
int main(int argc, char *argv[]){

     // The binary file to read from 
     FILE* binaryFile; 
     // The binary file to write to 
     FILE* outputFile; 
     // The name of the input binary file. Initially set to null. 
     char* binaryFileName = NULL; 
     // The name of the output binary file 
     char* outputFileName = NULL; 
     // The number of processes 
     int processCount;  
     
    if (argc != 7) {
        fprintf(stderr, "Usage: psort -n <number of processes> -f <inputfile> -o <outputfile>\n");
        exit(1);
    }
    /* read in arguments */
    int ch;  
    while ((ch = getopt(argc, argv, "n:f:o:")) != -1) 
    {
        switch(ch) 
        {
        case 'n': 
            processCount = strtol(optarg, NULL, 10); 
            if(processCount <= 0)
     	    { 
                fprintf(stderr, "Number of Child Processes cannot be zero\n");
                exit(0);
            }
            break; 
        case 'f':
            binaryFileName = optarg; 
            break; 
        case 'o':
            outputFileName = optarg; 
            break; 
        default:
            fprintf(stderr, "Usage: psort -n <number of processes> -f <inputfile> -o <outputfile>\n");
            exit(1);
        }
    }  
     

     // This is the number of records in the file  
     int recordCount = get_file_size(binaryFileName) / sizeof(struct rec);
     // If the number of records is zero, then this file is empty. 
     if(recordCount == 0)
         exit(0); 
     // This is the optimal (most even) assignments of records per process      
     int* optimAssign = optimizeSplit(&processCount, recordCount); 
     // This is the the value we want to seek by  
     int seek = 0; 
     // This is the 2D Array of file descriptors
     int fd[processCount][2];  

     for(int i = 0;i<processCount;i++)
     { 
          // Create the pipe between the child and the parent 
          Pipe(fd[i]); 
         
          // Create new child process 
          int result = Fork(); 
  	  
          if(result == 0)
          {
               // This is the code executed by the child process 
               struct rec readFromFile[optimAssign[i]]; 
               if(close(fd[i][0]) == -1) 
                  perror("close in child"); 

	       binaryFile = Fopen(binaryFileName, "rb");

	      // Increment seek to correct value 
	      for(int k = 0;k<i;k++)     
                  seek+=optimAssign[k];
              
              // Seek to the correct place in the file 
              if(fseek(binaryFile, sizeof(struct rec) * seek, SEEK_SET) == -1) 
	          perror("fseek");
 
              // Read from the binary file 
              if(fread(&readFromFile, sizeof(struct rec),
              optimAssign[i], binaryFile) == 0) 
	      {
	           perror("fread");     
		   exit(1); 
              }               

   	      // Now lets sort the array using qsort() 
	      qsort(readFromFile, optimAssign[i], 
                    sizeof(struct rec), compare_freq); 
 	      // Now we have a sorted array.   

	      for(int w = 0;w<optimAssign[i];w++)
	      {
                  // Write each struct to the pipe 
	          int e = write(fd[i][1], &readFromFile[w], 
		  sizeof(struct rec)); 
	      	  if(e == -1)
	              perror("write");
              }
	
	      // Close the pipe after child is done writing 
              if(close(fd[i][1]) == -1) 
		    perror("close"); 

	      Fclose(binaryFile); 

	      free(optimAssign);
	      exit(0); 
	  }
          else 
	  {
              // This is the code executed by the Parent process 
	      if(close(fd[i][1]) == -1) 
                  perror("close"); 
          }

	}

        struct rec nullStruct; 
        nullStruct.freq = -1; 
	struct rec parentRead[processCount]; 
	int counter = 0; 
	struct rec finalOutput[recordCount];

        // Read one struct from each pipe 
	for(int v = 0;v<processCount;v++)
        {
            if(read(fd[v][0], &parentRead[v], sizeof(struct rec)) == 0)
	        parentRead[v] = nullStruct;   
	}
	
	// Begin merging process 
         
        while(counter != recordCount)
        {
	    struct rec min = parentRead[0];  
            int minIndex = -1; 
            int t = 0; 
            while(parentRead[t].freq == -1)
	        t++; 
	   
            minIndex = t; 
            min = parentRead[t]; 

            // Find the minimum struct in the parentRead array 
    	    findMin(parentRead, &minIndex, &min, processCount);

	    finalOutput[counter] = min;
            if(read(fd[minIndex][0], &parentRead[minIndex], 
                 sizeof(struct rec)) == 0)
	    { 
	        // Reading failed, so close this pipe 
                if(close(fd[minIndex][0]) == -1) 
		    perror("close");  
		parentRead[minIndex] = nullStruct;  
	    }

            counter++;  
	}
	 
        outputFile = Fopen(outputFileName, "wb");

	// Write all structs to the output file 
        writeToOutputFile(finalOutput, recordCount, outputFile);

        // Close the output binary file 
        Fclose(outputFile);

       // Free the dynamically-allocated memory. 
       free(optimAssign); 

       int status; 
       wait(&status); 
       if(!WIFEXITED(status))
           fprintf(stderr, "Child terminated abnormally\n");

}


