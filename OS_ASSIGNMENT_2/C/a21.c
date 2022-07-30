#include<sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
 #include<pthread.h>
 #include <time.h>
 //monitoring//////////////////////////////////////////////////////////
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
    	
    struct condition_flag *v=(struct condition_flag*)x;
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
                    shmPtr1[0]=burst_time;
                    break;
                }
                if(a[v->process_id -1].process_go==0){
                    v->process_run=0;
                    clock_gettime(CLOCK_REALTIME, &e);
                    burst_time+=(e.tv_sec-s.tv_sec)+(e.tv_nsec-s.tv_nsec)/billion;
                   shmPtr1[0]=burst_time;
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
int ** dynamicm_memo;
 
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
      
 
      dynamicm_memo[data -> which_thread] = realloc(dynamicm_memo[data -> which_thread], (dynamicm_memo[data -> which_thread][0] + 1) * sizeof(int));
      dynamicm_memo[data -> which_thread][dynamicm_memo[data -> which_thread][0]] = num;
      dynamicm_memo[data -> which_thread][0]++;
      // store number
 
     // printf("7"); //
    }
    ///printf("8");
  }
  //printf("9");
 
}
//process_end reading
int main(int argc, char *argv[]) {
			//1st child process
			//printf("starting 1\n");
			struct condition_flag *v=(struct condition_flag*)malloc(sizeof(struct condition_flag));
			v->process_id=1;
			v->process_run=0;
			v->process_end=0;
			printf("C1 PROCESS CREATED\n");
			pthread_t t;
			pthread_create(&t,NULL,control_thread,(void*)v);    
					int size;
					int s;
					int shmid;
					long int *shmPtr;
					if (argc != 3) {
						printf("Usage: ./sudoku.out grid_size inputfile");
						exit(-1);
					}
				
					size = atoi(argv[1]);
					//long long int grid[size];
					//read_grid_from_file(size, argv[2], grid);
					//long long int sum1=0;
					//print_grid(size, grid);
					/*for(int i=0;i<size;i++){
					sum1=sum1+grid[i];
					}*/
					//printf("1 1");
				while(v->process_run==0){
					usleep(1000);
				    }
				int num_char = 0;
			  	int num_thr = 10;
			    	int num_numbers = atoi(argv[1]);
			    num_char = get_num_char(argv[2]);
			    //for(int num_thr=1;num_thr<num_numbers;num_thr++){**here
			    dynamicm_memo = (int ** ) malloc(num_thr * sizeof(int * ));
			 
			    //printf("12");
			    //  int num_thr=i;
			    pthread_t threads[num_thr];
			 
			    // dynamicm_memo[i-1]=(int **) malloc((i) *sizeof(int *));
			 
			    //creating threads
			    // pthread_create(&threads[0],NULL,check_column,(void *)param0);
			    for (int j = 0; j < num_thr; j++) {
			    //printf(" 1 2");
			    while(v->process_run==0){
					usleep(1000);
				    }
			      //printf("13");
			      dynamicm_memo[j] = (int * ) malloc((1) * sizeof(int));
			 
			      dynamicm_memo[j][0] = 1; 
			      parameters * param0 = (parameters * ) malloc(sizeof(parameters));
			 
				//assigning parameter to pass to threads
				param0 -> filename = argv[2]; param0 -> num_thread = num_thr; param0 -> num_chars = num_char; param0 -> which_thread = j; 
				pthread_create( & threads[j], NULL, fun, (void * ) param0);
			      }
			     // printf(" 1 3");
			      while(v->process_run==0){
					usleep(1000);
				    }
				    
				    
			      for (int j = 0; j < num_thr; j++) {
			      while(v->process_run==0){
					usleep(1000);
				    }
				//printf("14");
				pthread_join(threads[j], NULL);
			      }
			      
			      while(v->process_run==0){
					usleep(1000);
				    }
			 	long long int grid[num_numbers];
			 	int i=0;
			      //printf("15");
			     // printf("%d ->numthread\n", num_thr);
			 
			 
				printf("\n");
					//printf("%lld\n",sum1);
					if((shmid = shmget(key1,sizeof(grid),0666 | IPC_CREAT))== -1 ){
						printf("Parent error shmid");
						exit(3);
						}
			;		shmPtr = shmat(shmid, 0, 0);
					if(shmPtr == (int*)-1){
						printf("Parent error shmptr");
						exit(4);
					}
				for (int j = 0; j < num_thr; j++) {
				while(v->process_run==0){
					usleep(1000);
				    }
				//printf("part - %d \n", j);
					for (int k = 1; k < dynamicm_memo[j][0]; k++) {
					while(v->process_run==0){
					usleep(1000);
				    }
				  	//printf("%d ", dynamicm_memo[j][k]);
				  	shmPtr[i]=dynamicm_memo[j][k];
				  	i++;
					}
					
			      }
					//for(int n =0 ; n< size; n++){
						//shmPtr[n] = grid[n];
						//}
					//printf("Parent is Writing...");
					//flag=1
					printf("P1 Process Finished\n");
        				v->process_end=1;
        				v->process_run=0;
        				pthread_join(t,NULL);
 
}
