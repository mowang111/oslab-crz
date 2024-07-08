#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

const int MAX_BUFFER = 10;
const int N = 500;

/*semaphore*/
sem_t *empty, *full, *mutex;

/*shared memery*/
int shmid;
key_t key = 1234;
int *shm;

void producer(){
    int i;
    for(i = 0; i < N; i++){
        sem_wait(empty);
        sem_wait(mutex);
        shm[i] = i;
        sem_post(mutex);
        sem_post(full);
    }
    return;
}

int main(){
    /*create shared memory*/
    shmid = shmget(key, sizeof(int)*N, 0666);
    if(shmid < 0){
        printf("shmget error!\n");
        return 1;
    }
    shm = shmat(shmid, NULL, 0);
    if(shm == (int *)-1){
        printf("shm error!\n");
        return 1;
    }

    /*initialize semaphore*/
    /*empty = sem_open("myempty", O_CREAT, 0666, MAX_BUFFER);
    full = sem_open("myfull", O_CREAT, 0666, 0);
    mutex = sem_open("mymutex", O_CREAT, 0666, 1);*/
    empty = sem_open("myempty", MAX_BUFFER);
    full = sem_open("myfull", 0);
    mutex = sem_open("mymutex", 1);

    producer();

    sem_unlink("myempty");
    sem_unlink("myfull");
    sem_unlink("mymutex");
    return 0;
}