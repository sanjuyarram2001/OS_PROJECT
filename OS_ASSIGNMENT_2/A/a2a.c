#include<sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
 #include<pthread.h>
 #include<time.h>
 
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
int ** threadmemo;
 
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
      threadmemo[data -> which_thread] = realloc(threadmemo[data -> which_thread], (threadmemo[data -> which_thread][0] + 1) * sizeof(int));
      threadmemo[data -> which_thread][threadmemo[data -> which_thread][0]] = num;
      threadmemo[data -> which_thread][0]++;
      // store number
 
     // printf("7"); //
    }
    ///printf("8");
  }
  //printf("9");
 
}
//end reading
int main(int argc, char *argv[]) {
//1st child process
//printf("starting 1\n");
clock_t t1;
t1=clock();
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
		int num_char = 0;
   int num_thr = 10;
    int num_numbers = atoi(argv[1]);
    num_char = get_num_char(argv[2]);
   double times[45];
    t1=clock()-t1;
    
    for(int num_thr=1;num_thr<45;num_thr++){	//**here
    clock_t t;
    t=clock();
    threadmemo = (int ** ) malloc(num_thr * sizeof(int * ));
 
    //printf("12");
    //  int num_thr=i;
    pthread_t threads[num_thr];
 
    // threadmemo[i-1]=(int **) malloc((i) *sizeof(int *));
 
    //creating threads
    // pthread_create(&threads[0],NULL,check_column,(void *)param0);
    for (int j = 0; j < num_thr; j++) {
      //printf("13");
      threadmemo[j] = (int * ) malloc((1) * sizeof(int));
 
      threadmemo[j][0] = 1; 
      parameters * param0 = (parameters * ) malloc(sizeof(parameters));
 
        //assigning parameter to pass to threads
        param0 -> filename = argv[2]; param0 -> num_thread = num_thr; param0 -> num_chars = num_char; param0 -> which_thread = j; 
        pthread_create( & threads[j], NULL, fun, (void * ) param0);
      }
      for (int j = 0; j < num_thr; j++) {
        //printf("14");
        pthread_join(threads[j], NULL);
      }
 	long long int grid[num_numbers];
 	int i=0;
      //printf("15");
     // printf("%d ->numthread\n", num_thr);
		//printf("%lld\n",sum1);
		if((shmid = shmget(key,sizeof(grid),0666 | IPC_CREAT))== -1 ){
			printf("Parent error shmid");
			exit(3);
			}
;		shmPtr = shmat(shmid, 0, 0);
		if(shmPtr == (int*)-1){
			printf("Parent error shmptr");
			exit(4);
		}
	for (int j = 0; j < num_thr; j++) {
        //printf("part - %d \n", j);
        	for (int k = 1; k < threadmemo[j][0]; k++) {
          	//printf("%d ", threadmemo[j][k]);
          	shmPtr[i]=threadmemo[j][k];
          	i++;
        	}
      }
      
	t=clock()-t;
	double time_taken=(((double)t)/CLOCKS_PER_SEC)+(((double)t1)/CLOCKS_PER_SEC);
	times[(num_thr)-1]=time_taken;	//for(int n =0 ; n< size; n++){
			//shmPtr[n] = grid[n];
			//}
		//printf("Parent is Writing...");
		//flag=1
		}
		for(int i=0;i<45;i++) printf("%f",times[i]);
 
}