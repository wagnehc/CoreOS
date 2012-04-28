
/*

  AUTHORS:  Henry Wagner AND Matt Echeverria

  Date: 04/27/2012
  Updated: 04/28/2012

  This program uses 4 threads to read from a file, count the words, lines and bytes in a file.

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

void *countBytes(void *arg);
void *countWords(void *arg);
void *countLines(void *arg);
void barrier();
void barrier2();
void barrier3();
void eat();
void writeToBuffer();
void charCount();
void wordCount();
void lineCount();
void CharCounter();
void WordCounter();
void LineCounter();
void writer();
struct timeval before, after;

char buffer[5][2050];
sem_t y[5], rcount[5], wsem[5], rsem[5], philosopher, barriersem;
int size = 0, bytes = 0, words = 0, lines = 0, barrier_count = 0, barrier_count2 = 0, barrier_count3 = 0, i2;
int readcount[5], writecount[5], count;
bool ByteReaderLock[5], WordReaderLock[5], LineReaderLock[5];

FILE *file;
volatile size_t result;
bool writerComplete = false;

int main(){
	   
	gettimeofday(&before, NULL);
	int i;
	
	/* Initialization of all semaphores, counters, and booleans */
	for( i = 0; i < 5; i++){
		sem_init(&y[i], 0, 1);
		sem_init(&rsem[i],0,1);
		sem_init(&wsem[i], 0, 1);
		readcount[i] = 0;
		writecount[i] = 0;
		sem_init(&rcount[i],0,1);
		WordReaderLock[i] = false;
		LineReaderLock[i] = false;
	}
	sem_init(&philosopher, 0, 2);
	sem_init(&barriersem, 0, 1);
	pthread_t bytethread, wordthread, linethread;
	
	/* file open and integrity check */
	file = fopen("testfile.txt", "r");
	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	rewind (file);

	/* creation of three alternate threads */
	if(pthread_create( &bytethread, NULL, countBytes, NULL)){
		printf("error creating thread.");
		abort();
	}
	if(pthread_create( &wordthread, NULL, countWords, NULL)){
		printf("error creating thread.");
		abort();
	}
	if(pthread_create( &linethread, NULL, countLines, NULL)){
		printf("error creating thread.");
		abort();
	}

	/* Main thread execution */
	barrier();
	eat();
	barrier2();
	writeToBuffer();
	barrier3();
	eat();


	/* Join alternate threads back to main thread before completion of programs */
	if (pthread_join(bytethread, NULL)) {
		printf("error join thread");
		abort();
	}
	if (pthread_join(wordthread, NULL)) {
		printf("error join thread");
		abort();
	}
	if (pthread_join(linethread, NULL)) {
		printf("error join thread");
		abort();
	}
	  
	/* Report all calculations */
	printf("Total number of Characters is:  %i\n", bytes);
	printf("Total number of Words is: %i\n", words);
	printf("total number of Lines is: %i\n", lines);
	
	/* Report total time of execution */
	gettimeofday(&after, NULL);
	count = (after.tv_sec - before.tv_sec) * 1e6;
    count += (after.tv_usec - before.tv_usec);
    printf("Total execution time in usec: %d\n", count);

	return 0;
}

void *countWords(void *arg){
	barrier();
	eat();
	barrier2();
	wordCount();
	barrier3();
	eat();
}

void *countBytes(void *arg){
	barrier();
	eat();
	barrier2();
	charCount();
	barrier3();
	eat();
}
void *countLines(void *arg){
	barrier();
	eat();
	barrier2();
	lineCount();
	barrier3();
	eat();
}

void barrier(){
	sem_wait(&barriersem);
	barrier_count++;
	sem_post(&barriersem);
	while (barrier_count != 4){}
}

void barrier2(){
	sem_wait(&barriersem);
	barrier_count2++;
	sem_post(&barriersem);
	while (barrier_count2 != 4){}
}
   
void barrier3(){
	sem_wait(&barriersem);
	barrier_count3++;
	sem_post(&barriersem);
    while (barrier_count3 != 4){}    
}

void eat(){
	sem_wait(&philosopher);
	;/* eating */
	sem_post(&philosopher);
}

/* Function to continue Character counting until writer has reached the end of the document */
void charCount(){
	while(!writerComplete){
		CharCounter();
	}
	// once the writer is finished complete one more walk through the buffers
	CharCounter();
}

/* Function to continue Word counting until writer has reached the end of the document */
void wordCount(){
	while(!writerComplete){
		WordCounter();
	}
	// once the writer is finished complete one more walk through the buffers
	WordCounter();
}

/* Function to continue Word counting until writer has reached the end of the document */
void lineCount(){
	while(!writerComplete){
		LineCounter();
	}
	// once the writer is finished complete one more walk through the buffers
	LineCounter();
}


void writeToBuffer(){
	int i = 0;

		while(!writerComplete){
			sem_wait(&rsem[i]);			
				sem_wait(&wsem[i]);
					if ( buffer[i][2049] == '\0' && !WordReaderLock[i] && !LineReaderLock[i]){
						buffer[i][2049] = '1';
						result = fread(buffer[i],1,2048,file);
						WordReaderLock[i] = true;
						LineReaderLock[i] = true;
						buffer[i][result] = '\0';
						if (ferror (file));
					}
				sem_post(&wsem[i]);
			sem_post(&rsem[i]);
				
			if(feof(file)){
				writerComplete = true;
			}

			i++;
			if( i > 4 ){
				i = 0;
			}
		}
	fclose(file);
}


void CharCounter(){
	int i = 0, j;
	char* character;
	for(i = 0; i < 5; i++){	
		sem_wait(&y[i]);
			sem_wait(&rsem[i]);
				sem_wait(&rcount[i]);
					readcount[i]++;
					if(readcount[i] == 1){
						sem_wait(&wsem[i]);
					}
				sem_post(&rcount[i]);
				
				character = buffer[i];
				if(buffer[i][2049] != '\0'){
					for (j = 0; j < 2048; j++){
						if (*character != '\0'){
							bytes++;
						}
						else{
							break;
						}
						character++;
					}
					buffer[i][2049] = '\0';
				}	
			sem_post(&rsem[i]);
		sem_post(&y[i]);			

		sem_wait(&rcount[i]);
			readcount[i]--;
			if(readcount[i] == 0){
				sem_post(&wsem[i]);
			}
		sem_post(&rcount[i]);
	}
}


 void LineCounter(){
	int i = 0, j;
	char* c;
	
	for(i = 0; i < 5; i++){	
		sem_wait(&y[i]);	
			sem_wait(&rsem[i]);
				sem_wait(&rcount[i]);
					readcount[i]++;
						if(readcount[i] == 1){
						sem_wait(&wsem[i]);
						}
					sem_post(&rcount[i]);

					c = buffer[i];
					if(LineReaderLock[i]){
						for (j = 0; j < 2047; j++){
							if (*c == '\0'){
								break;
							}
							else if (*c == '\n'){
								lines++;
							}
							c++;
						}
						LineReaderLock[i] = false;
					}
			sem_post(&rsem[i]);
		sem_post(&y[i]);	

		sem_wait(&rcount[i]);
			readcount[i]--;
			if(readcount[i] == 0){
				sem_post(&wsem[i]);
			}
		sem_post(&rcount[i]);
	}
}

	
void WordCounter(){

	int i = 0, j;
	char* c;
	bool word = false;
	
	for(i = 0; i < 5; i++){	
		sem_wait(&y[i]);	
			sem_wait(&rsem[i]);
				sem_wait(&rcount[i]);
					readcount[i]++;
					if(readcount[i] == 1){
						sem_wait(&wsem[i]);
					}
				sem_post(&rcount[i]);
			
				c = buffer[i];
				if(WordReaderLock[i]){
					for (j = 0; j < 2048; j++){
						if (*c == '\0'){
							break;
						}
						else if (*c != ' '){
							word = true;
						}
						if (*c == ' '){
							word = false;
						}
						if ( !word && (*(c - 1) != ' ' ||  *(c - 1) != '\0' )){
							words++;
						}
					}
					WordReaderLock[i] = false;
				}
			sem_post(&rsem[i]);	
		sem_post(&y[i]);				
		
		sem_wait(&rcount[i]);
			readcount[i]--;
			if(readcount[i] == 0){
				sem_post(&wsem[i]);
			}
		sem_post(&rcount[i]);
	}
}


	
	


