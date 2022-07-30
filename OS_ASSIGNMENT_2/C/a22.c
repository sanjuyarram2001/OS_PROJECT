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
 
 
 //monitoring//////////////////////////////////////////////////////////
 #define million 1000000.0
#define billion 1000000000.0
 
 struct control_flag{
    int process_id;
    int process_run;
    int process_end;
};
struct process{
    int process_go;
    int process_done;
};
 
 void* control_thread(void* x){
 	int shmid1;
 	key_t key1;
 	if((key1=ftok("a23.c",'D'))==-1){
        perror("Error ftok");
        exit(1);
    	}
    	if((shmid1=shmget(key1,2*sizeof(int),0666|IPC_CREAT))==-1){
        perror("Error shmget");
        exit(1);
    	}
    	int * shmPtr1=(int *)shmat(shmid1,NULL,0);
    struct control_flag *v=(struct control_flag*)x;
    printf("control_thread  for C%d process started\n",v->process_id);
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
    struct timespec process_begin,process_end;
    clock_gettime(CLOCK_REALTIME, &process_begin);
    struct timespec s,e;
    double burst_time=0;
    while(a[v->process_id -1].process_done==0){
        if(a[v->process_id -1].process_go==1){
            v->process_run=1;
            clock_gettime(CLOCK_REALTIME,&s);
            while(1){
                usleep(1000);
                if(v->process_end==1){
                    a[v->process_id -1].process_done=1;
                    clock_gettime(CLOCK_REALTIME, &e);
                    burst_time+=(e.tv_sec-s.tv_sec)+(e.tv_nsec-s.tv_nsec)/billion;
                    //shmPtr1[1]=burst_time;
                    break;
                }
                if(a[v->process_id -1].process_go==0){
                    v->process_run=0;
                    clock_gettime(CLOCK_REALTIME, &e);
                    burst_time+=(e.tv_sec-s.tv_sec)+(e.tv_nsec-s.tv_nsec)/billion;
                   // shmPtr1[1]=burst_time;
                    break;
                }
            }
        }
        
        usleep(1000);
    }
    clock_gettime(CLOCK_REALTIME,&process_end);
    double time_spent=(process_end.tv_sec-process_begin.tv_sec)+(process_end.tv_nsec-process_begin.tv_nsec)/billion;
    printf("Turn Around Time - C%d Process=%lf\n",v->process_id,time_spent);
    printf("Total Burst Time - C%d Process=%lf\n",v->process_id,burst_time);
    printf("Waiting Time - C%d Process=%lf\n",v->process_id,time_spent-burst_time);
    pthread_exit(NULL);
}
//ending monitoring////////////////////////////////////////////////////////////////////////////
  #define key1 2044
 long long int sum=0;
 
 pthread_mutex_t lock;
 void *func(void *val)
 {
 	long long int* data = (long long int*)val;
 	pthread_mutex_lock(&lock);
 	sum+=(*data);
 	pthread_mutex_unlock(&lock);
 }
 void *func1(void *val)
 {
 	long int* data = ((long int*)val);
 	pthread_mutex_lock(&lock);
 	sum+=(*data);
 	pthread_mutex_unlock(&lock);
 	free(val);
 }
void read_grid_from_file(int size, char *ip_file, long long int grid[]) {
	FILE *fp;
	int i, j;
	fp = fopen(ip_file, "r");
	for (i=0; i<size; i++) { 
	fscanf(fp, "%lld", &grid[i]);
	}
} 
 
 
void print_grid(int size, long long int grid[]) {
	int i, j;
	/* The segment below prints the grid in a standard format. Do not change */
	for (i=0; i<size; i++) printf("%lld\t", grid[i]);
}
 
 
 //for reading
 int get_num_char(char * ip_file) {
    FILE * fp;
    int i, j;
    int pos;
    //printf("11");
    fp = fopen(ip_file, "r");
    fseek(fp, 0l, SEEK_END);
    pos = ftell(fp);
    fclose(fp);
    return pos;
  }
int ** dynamic_memo;
 
//fseek(fid, 0l, SEEK_END);
typedef struct // creating struct to pass as an input to thread
{
  char * filename;
  int which_thread;
  int num_thread;
  int num_chars;
}
parameters;
 
void * fun(void * params) {
  parameters * data = (parameters * ) params;
  //printf("2");
  FILE * fp;
  int i, j;
  int pos;
  fp = fopen(data -> filename, "r");
  int l = ((data -> num_chars) / (data -> num_thread)) * (data -> which_thread);
  int r;
  if (data -> which_thread + 1 == data -> num_thread) {
    r = data -> num_chars;
  } else r = ((data -> num_chars) / (data -> num_thread)) * (data -> which_thread + 1);
  fseek(fp, l, SEEK_SET);
  //printf("3");
  while (ftell(fp) < r) {
    //printf("4");
    int pos = ftell(fp);
    int num = -1;
    if (pos == 0) {
      num = 0;
      while (1) {
       // printf("1");
        char ch = fgetc(fp);
        if (ch != ' ' && ch != EOF)
          num = num * 10 + (ch - '0');
        else {
          fseek(fp, -1, SEEK_CUR);
          break;
        }
      }
    } else {
      char ch = fgetc(fp);
      if (ch == ' ') {
        num = 0;
        while (1) {
          //printf("5");
          char ch = fgetc(fp);
          if (ch != ' ' && ch != EOF)
            num = num * 10 + (ch - '0');
          else {
            fseek(fp, -1, SEEK_CUR);
            break;
          }
        }
      }
 
      //printf("6");
    }
 
    if (num != -1) {
      //printf("num: %d \t", num);
      //
     
      dynamic_memo[data -> which_thread] = realloc(dynamic_memo[data -> which_thread], (dynamic_memo[data -> which_thread][0] + 1) * sizeof(int));
      dynamic_memo[data -> which_thread][dynamic_memo[data -> which_thread][0]] = num;
      dynamic_memo[data -> which_thread][0]++;
      // store number
 
     // printf("7"); //
    }
    ///printf("8");
  }
  //printf("9");
 
}
//process_end reading
int main(int argc, char *argv[]) {
		struct control_flag *v=(struct control_flag*)malloc(sizeof(struct control_flag));
		v->process_id=2;
		v->process_run=0;
		v->process_end=0;
		printf("C2 PROCESS CREATED\n");
		pthread_t t;
		pthread_create(&t,NULL,control_thread,(void*)v);    
			int size;
			int s;
			int shmid;
				long int *shmPtr;
				size = atoi(argv[1]);
				long long  int gridn[size];
				while(v->process_run==0){
					usleep(1000);
				    }
				if((shmid =shmget(key1,sizeof(gridn),0))== -1 ){
					printf("child error shmid");
					exit(1);
					}
				shmPtr = shmat(shmid, 0, 0);
				while(v->process_run==0){
					usleep(1000);
				    }
				if(shmPtr == (int*)-1){
				printf("child error shmptr");
					exit(2);}
				for(int n =0 ; n< size; n++){
					gridn[n] = shmPtr[n];
					}
				//printf("Child is reading...");
				
				//summing
			//pthread_t t[size];
		//	int i;
			pthread_mutex_init(&lock,NULL);
			
			for(int numt=5;numt<=5;numt++){
			while(v->process_run==0){
					usleep(1000);
				    }
				    
			pthread_t t[numt];
			for(int i=0;i<numt-1;i++)
			{
			while(v->process_run==0){
					usleep(1000);
				    }
				long long int *tempsum = malloc(sizeof(*tempsum));
		 
			*tempsum=0;
				for(int j=(size/numt)*i;j<((size/numt)*(i+1));j++)
				{
				while(v->process_run==0){
					usleep(1000);
				    }
				*tempsum+=gridn[j]	;
				}
				
				pthread_create(&t[i],NULL,func1,(void*)tempsum);
			
			
			}
			while(v->process_run==0){
					usleep(1000);
				    }
			long long int *tempsum = malloc(sizeof(*tempsum));
		 
			*tempsum=0;
				
				
			for(int j=(size/numt)*(numt-1);j<size;j++)
				{
				while(v->process_run==0){
					usleep(1000);
				    }
					*tempsum+=gridn[j];
				}
				while(v->process_run==0){
					usleep(1000);
				    }
					pthread_create(&t[numt-1],NULL,func1,(void*)tempsum);
				for(int i=0;i<numt;i++)
			{
			while(v->process_run==0){
					usleep(1000);
				    }
				pthread_join(t[i],NULL);
			}
			while(v->process_run==0){
					usleep(1000);
				    }
			printf("%lld\n",sum);
			sum=0;
			/*for(int i=0;i<size;i++)
			{
				pthread_create(&t[i],NULL,func,(void*)&gridn[i]);
			
			}
			for(int i=0;i<size;i++)
			{
				pthread_join(t[i],NULL);
			}
			printf("%lld\n",sum);*/
			
				}
				printf("P2 Process Finished\n");
        			v->process_end=1;
        			v->process_run=0;
        			pthread_join(t,NULL);
				return 0;
}
