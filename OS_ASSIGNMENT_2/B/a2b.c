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
 
 
 
  #define key 2044
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
//end reading
double times[1000000];
int main(int argc, char *argv[]) {
clock_t t1;
t1=clock();
	int size;
	int s;
	int shmid;
		long int *shmPtr;
		size = atoi(argv[1]);
		long long  int gridn[size];
		if((shmid =shmget(key,sizeof(gridn),0))== -1 ){
			printf("child error shmid");
			exit(1);}
		shmPtr = shmat(shmid, 0, 0);
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
	 t1=clock()-t1;
	for(int numt=1;numt<=size;numt++){
	clock_t t2;
    	t2=clock();
	pthread_t t[numt];
	for(int i=0;i<numt-1;i++)
	{
		long long int *tempsum = malloc(sizeof(*tempsum));
 
        *tempsum=0;
		for(int j=(size/numt)*i;j<((size/numt)*(i+1));j++)
		{
		*tempsum+=gridn[j]	;
		}
		
		pthread_create(&t[i],NULL,func1,(void*)tempsum);
	
	
	}
	long long int *tempsum = malloc(sizeof(*tempsum));
 
        *tempsum=0;
		
		
	for(int j=(size/numt)*(numt-1);j<size;j++)
		{
			*tempsum+=gridn[j];
		}
			pthread_create(&t[numt-1],NULL,func1,(void*)tempsum);
		for(int i=0;i<numt;i++)
	{
		pthread_join(t[i],NULL);
	}
	//printf("%lld\n",sum);
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
	t2=clock()-t2;
	double newtime = ((double)t2)/CLOCKS_PER_SEC+((double)t1)/CLOCKS_PER_SEC;
	times[(numt)-1]=newtime;
		}
		for(int i=0;i<size;i++) printf("%f",times[i]);
		return 0;
		}
