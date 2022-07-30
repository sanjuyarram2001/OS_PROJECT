#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
 #include<pthread.h>
 
 
 #define million 1000000.0
#define billion 1000000000.0
 
 
 
struct condition_flag{
    int process_id;
    int process_run;
    int process_end;
};
struct process{
    int process_go;
    int process_done;
};
 
void Round_Robin(int q){
    key_t key;
    if((key=ftok("a23.c",'B'))==-1){
        perror("Error ftok");
        exit(1);
    }
    int shmid;
    if((shmid=shmget(key,2*sizeof(struct process),0666))==-1){
        perror("Error shmget");
        exit(1);
    }
    struct process *a=(struct process*)shmat(shmid,NULL,0);
    int i=0;
    struct timespec process_begin,process_end;
    while(!(a[0].process_done==1 && a[1].process_done==1)){
        if(a[i].process_done){
            i=(i+1)%2;
            continue;
        }
        a[i].process_go=1;
        clock_gettime(CLOCK_REALTIME, &process_begin);
        while(a[i].process_done==0){
            a[i].process_go=1;
            clock_gettime(CLOCK_REALTIME, &process_end);
            if((process_end.tv_sec-process_begin.tv_sec)*1000.0+(process_end.tv_nsec-process_begin.tv_nsec)/million>=q){
                a[i].process_go=0;
                break;
            }
            usleep(1000);
        }
        i=(i+1)%2;
    }
    printf("All process completed...\n");
} 
 
void prp(){
	int shmid1;
 	key_t key2;
 	if((key2=ftok("a23.c",'D'))==-1){
        perror("Error ftok");
        exit(1);
    	}
    	if((shmid1=shmget(key2,2*sizeof(int),0666|IPC_CREAT))==-1){
        perror("Error shmget");
        exit(1);
    	}
    	int * shmPtr1=(int *)shmat(shmid1,NULL,0);
    	
    	key_t key;
    if((key=ftok("a23.c",'B'))==-1){
        perror("Error ftok");
        exit(1);
    }
    	int shmid;
    if((shmid=shmget(key,2*sizeof(struct process),0666))==-1){
        perror("Error shmget");
        exit(1);
    }
    struct process *a=(struct process*)shmat(shmid,NULL,0);
    	int i=0;
    	while(!(a[0].process_done==1 && a[1].process_done==1)){
        if(a[i].process_done){
            i=(i+1)%2;
            continue;
        }
        
        while(a[i].process_done==0){
            a[i].process_go=1;
            int j=(j+1)%2;
            if(shmPtr1[i]>=shmPtr1[j]){
                a[i].process_go=0;
                break;
            }
            usleep(1000);
        }
        i=(i+1)%2;
    }
 
}
int main(int argc, char *argv[]){
		int size;
		int s;
		int shmid;
		long int *shmPtr;
		if (argc != 3) {
			printf("Usage: ./sudoku.out grid_size inputfile");
			exit(-1);
		}
		printf("Enter 1 for Round robin, 2 for Pre-emptive priority scheduling\n");
		int p; scanf("%d",&p);
		key_t key;
		    if((key=ftok("a23.c",'B'))==-1){
			perror("Error ftok");
			exit(1);
		    }
		   
		    if((shmid=shmget(key,2*sizeof(struct process),0666|IPC_CREAT))==-1){
			perror("Error shmget");
			exit(1);
		    }
		    struct process *a=(struct process*)shmat(shmid,NULL,0);
		    for(int i=0;i<2;i++){
			a[i].process_done=0;
			a[i].process_go=0;
		    }
	
		size = atoi(argv[1]);
		
		if(fork()==0){				//child1
			//execlp("./a22.out","./a22.out",argv[1],(char *)NULL);
			execlp("./a21.out","./a21.out",argv[1],argv[2],(char *)NULL);
		}
		else{
			if(fork()==0){			//child2
			//execlp("./a21.out","./a21.out",argv[1],argv[2],(char *)NULL);
			execlp("./a22.out","./a22.out",argv[1],(char *)NULL);
			}
			else{
			if(p==1) Round_Robin(1000);
			else if(p==2) prp();
			wait(NULL);
			wait(NULL);
			}
		}
	
 
}
