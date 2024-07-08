#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#define N 5
#define M 500

FILE *fp_write, *fp_read;
sem_t *empty, *full, *mutex;


void Producer(){
    int i;
    fflush(stdout);                                 
    for(i = 0; i < M; i++){
        sem_wait(empty);
        sem_wait(mutex);
        fwrite(&i, sizeof(int), 1, fp_write);
        fflush(fp_write);
        sem_post(mutex);
        sem_post(full);
    }
}

void Consumer(){
    int num;
    fflush(stdout);
    while(1){
        sem_wait(full);
        sem_wait(mutex);
        fread(&num, sizeof(int), 1, fp_read);
        printf("%d: %d\n", getpid(), num);
        fflush(stdout);
        sem_post(mutex);
        sem_post(empty);
    }
}

int main(){
    int i;
    fp_write = fopen("buffer.txt", "w+");
    fp_read = fopen("buffer.txt", "r");
    setvbuf(fp_read, NULL, _IONBF, 0);
    if(fp_write == NULL || fp_read == NULL){
        printf("File open error!\n");
        exit(1);
    }

    /* empty = sem_open("/myempty", O_CREAT, 0666, 10);*/
    empty = sem_open("/myempty", 10);

    if(!empty){
        printf("sem_open empty error!\n");
    }
    /* full = sem_open("/myfull", O_CREAT, 0666, 0);*/
    full = sem_open("/myfull", 0);
    if(!full){
        printf("sem_open full error!\n");
    }
    /* mutex = sem_open("/mymutex", O_CREAT, 0666, 1);*/
    mutex = sem_open("/mymutex", 1);
    if(!mutex){
        printf("sem_open mutex error!\n");
    }

    for(i = 0; i < N; i++){
        if(fork() == 0){
            Consumer();
            exit(0);
        }
    }

    Producer();

    for(i = 0; i < N; i++){
        wait(NULL);
    }

    fclose(fp_write);
    fclose(fp_read);
    sem_unlink("/myempty");
    sem_unlink("/myfull");
    sem_unlink("/mymutex");

    return 0;
}