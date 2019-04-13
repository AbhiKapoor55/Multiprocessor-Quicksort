#ifndef _HELPER_H
#define _HELPER_H

#define SIZE 44

struct rec {
    int freq;
    char word[SIZE];
};

int get_file_size(char *filename);
int compare_freq(const void *rec1, const void *rec2);
void Pipe(int* fd);
FILE* Fopen(char* file, char* type);
int Fork();
void *Malloc(int size);
void Fclose(FILE* binaryFile);
void writeToOutputFile(struct rec* finalOutput, int recordCount, FILE* outputFile);
void findMin(struct rec* parentRead, int* minIndex, struct rec* min, int processCount);
int* optimizeSplit(int* numOfProcesses, int numOfRecords); 


#endif /* _HELPER_H */
