#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#define MAXSIZE 1024

//definitions
int buffers[6][MAXSIZE];
int numOfFiles;
sem_t sems[3];

//struct for threads containing all information for merge
typedef struct{
	int index;
	int isFromFile;
	int isToFile;
	char file1[30];
	char file2[30];
	char outFile[15];
	int bufindex_1;
	int bufindex_2;
	int bufwriteindex;
}threadReadWrite;

//function to initialize semaphores with value 0
void init_sems(){
	int i;
	for(i=0;i<3;i++){
		  sem_init (&sems[i], 0, 0);
	}
}

//merge function which will be run by threads
void *mergeFunc(void *thrdstructt){
	threadReadWrite *thrdstruct = (threadReadWrite*) thrdstructt;
	//according to number of files and index of thread decide which ones should wait
	int indx=thrdstruct->index;
	if(numOfFiles==8 && indx>=4){
		printf("Thread %d waits\n",indx);
		sem_wait(&sems[indx-4]);
		sem_wait(&sems[indx-4]);		
	}
	else if(numOfFiles==4 && indx==2){
		printf("Thread %d waits\n",indx);
		sem_wait(&sems[0]);
		sem_wait(&sems[0]);
	}
	printf("Thread %d goes\n",indx);
	//to use later easily assign necessary values
	int fromFile=thrdstruct->isFromFile;
	int toFile=thrdstruct->isToFile;
	int rIndex1=thrdstruct->bufindex_1;
	int rIndex2=thrdstruct->bufindex_2;
	int wIndex=thrdstruct->bufwriteindex;	
	int n1=0,n2=0;
	FILE *read1; 
    	FILE *read2; 
	int num1,num2,start=0;
	/*if read from file open files and take total number of integers and first numbers
	if from buffer then calculate number of elements and take first numbers*/
	if(fromFile){
		read1=fopen(thrdstruct->file1, "r"); 
		read2=fopen(thrdstruct->file2, "r");
		fscanf(read1,"%d",&n1);
		fscanf(read2,"%d",&n2);	
		fscanf(read1,"%d",&num1);
		fscanf(read2,"%d",&num2);
	} 
	else{
		int h=0;
		while(buffers[rIndex1][h]!=0){
			n1++;h++;
		}
		h=0;
		while(buffers[rIndex2][h]!=0){
			n2++;h++;
		}
		n1++;n2++;
		num1=buffers[rIndex1][0];
		num2=buffers[rIndex2][0];
		start=1;
	}
	//if write to file open output file with specified name
	FILE *output;			
	if(toFile)
		output=fopen(thrdstruct->outFile,"a");
	int i=start,j=start,k=0;
	//loop for merge operation
	while (i<n1 && j <n2){
		//check which number is smaller and write that number to destination accordingly and took next number to compare
		if (num1 < num2){
			if(toFile)
				fprintf(output, "%d\n",num1);
			else
				buffers[wIndex][k++]=num1;
			if(fromFile){
				fscanf(read1,"%d",&num1);
				i++;
			}
			else
				num1=buffers[rIndex1][i++];				
		}
		else{
			if(toFile)
				fprintf(output, "%d\n",num2);
			else
				buffers[wIndex][k++]=num2;
			if(fromFile){
				fscanf(read2,"%d",&num2);
				j++;
			}
			else
				num2=buffers[rIndex2][j++];
		}

	}   
	//if ther eare numbers left put them in the end accordingly
    while (i < n1){
		if(toFile)
			fprintf(output, "%d\n",num1);
		else
			buffers[wIndex][k++]=num1;
		if(fromFile){
			fscanf(read1,"%d",&num1);
			i++;
		}
		else{
			num1=buffers[rIndex1][i++];
		}
	}
    	while (j < n2){
		if(toFile)
			fprintf(output, "%d\n",num2);
		else
			buffers[wIndex][k++]=num2;
		if(fromFile){
			fscanf(read2,"%d",&num2);
			j++;
		}
		else{
			num2=buffers[rIndex2][j++];
		}
	}
	printf("Thread %d ends...\n",indx);
	//again according to number of files and indices post necessary semaphore to notify that it finished the operation
	if(numOfFiles==8){
		if(indx<2)
			sem_post(&sems[0]);
		else if(indx<4)
			sem_post(&sems[1]);
		else if(indx<6)
			sem_post(&sems[2]);
		
	}
	if(numOfFiles==4){
		sem_post(&sems[0]);
	}
}

int main(int argc, char* argv[]){
	//check if least number of arguments is given or not
	if (argc < 7){
		printf("\nUsage: a.out -n #num file1 file2 ... -o outfile (num=2,4 or 8)\n");
		return 1;
	}
	//initialize semaphores and assign number of buffers and threads to be used
	init_sems();
	numOfFiles=atoi(argv[2]);
	int numOfBufs=numOfFiles-2;
	int numOfThrds=numOfFiles-1;
	//variables for thread indices and structs
	pthread_t threadTable[numOfThrds];
	threadReadWrite* thrdstructs[numOfThrds];
	int i=0,j=0,k=0,h=0;
	//loop to create all threads
	for (i = 0; i < numOfThrds; i++){
		printf("Thread %d\n",i);
		thrdstructs[i]=(threadReadWrite*) malloc (sizeof (threadReadWrite));
		/*assign struct values accordingly while creating threads like 
		thread will read from files or buffers (which buffers also) or write to file or buffer*/
		thrdstructs[i]->index=i;
		if(numOfFiles!=2 && k==numOfFiles){
			//thrdstructs[i]->isFromFile=0;
			thrdstructs[i]->bufindex_1=h++;
			thrdstructs[i]->bufindex_2=h++;
		}
		else{
			thrdstructs[i]->isFromFile=1;
			strcpy(thrdstructs[i]->file1,argv[3+k++]);
			strcpy(thrdstructs[i]->file2,argv[3+k++]);
		}
		if(i==numOfThrds-1){
			thrdstructs[i]->isToFile=1;
			strcpy(thrdstructs[i]->outFile,argv[argc-1]);
		}
		else{
			//thrdstructs[i]->isToFile=0;
			thrdstructs[i]->bufwriteindex=j++;
		}
		//create each thread with given struct
		int status = pthread_create(threadTable + i, NULL,mergeFunc, thrdstructs[i]);
		if (status != 0)		{
			fprintf(stderr, "Failed to create thread %d: %s\n",i, strerror(status));
			return 1;
		}
	} 
	//wait for all threads to join
	for (i = 0; i < numOfThrds; i++){
		int *exitCodePtr;
		int stat = pthread_join(threadTable[i],(void **)&exitCodePtr);
	}
	return 0;
}
