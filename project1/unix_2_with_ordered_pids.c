#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

//function to print indents according to depth level for all
void indent(int depth){
	int i;
	for(i=0;i<depth;i++){
		printf(" ");		
	}
}

int main(void)
{
	int depth=0;
	int input;
	scanf("%d", &input);	//take depth of the tree from the user
	int temp=input;	
	int traverseNum=1;	//assign traverseNum as 1 for main process
	pid_t child1,child2;
	while(input>0){	//loop till depth level is reached
		indent(depth);	//before printing info indent according to level the node in
		printf("[%d] pid %ld, ppid %ld\n",(traverseNum),(long)getpid(),(long)getppid()); //info of process
		if(input==1){
			break;
		}
		if(abs(temp-input)==depth){	//to construct binary tree check each node's depth and fork accordingly
			child1=fork();	//first child - left
			if(child1>0){	//if parent, first print left child's info then fork second child - right and print its info
				indent(depth);
				printf("[%d] pid %ld, created left child with pid %ld\n",(traverseNum),(long)getpid(),(long)child1);
				child2=fork();
				if(child2>0){
					indent(depth);
					printf("[%d] pid %ld, created right child with pid %ld\n",(traverseNum),(long)getpid(),(long)child2);				
					break;
				}
				else{
					traverseNum=2*traverseNum+1;	//for right childs assign traverseNum as 2*(parent's)+1
					depth++;	//increment depth
					sleep(traverseNum);	//to make each process sleep for some time
					
				}
			}
			else{
				traverseNum*=2;	//for left child assign traerseNum as 2*(parent's)
				depth++;	//increment depth
				sleep(traverseNum);	//to make each process sleep for some time
			}
		}
		input--;	//decrement input
	} 
	sleep(3);
	int trNum;
	char side[6];
	pid_t cpid;
	while((cpid=wait(&trNum))>0){	//wait for all children and take return value as trNum
 		if ( WIFEXITED(trNum) ) {
        		int stat = WEXITSTATUS(trNum);	//take traverseNum of child as status
			if(stat%2==0)	//determine whether it is right or left child
				strcpy(side,"left");
			else
				strcpy(side,"right");
			indent(depth);	//print exit info
			printf("[%d] %s child %ld of %ld exited with status %d\n",(traverseNum),side,(long)cpid,(long)getpid(),stat);
		}				
	}
	return traverseNum;	//return traverse number
}
