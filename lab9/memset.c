#include <stdio.h>
#include <pthread.h>

typedef struct{
    pthread_t thread;
    int testpos;
    int testsize;
    char* startaddr;
} testinfo;

testinfo info[10];
int threadnum = 0;

void memtest(void* arg){
    testinfo* threadinfo = (testinfo*)arg;
    char* startaddr = threadinfo->startaddr;
    int testsize = threadinfo->testsize;
    char* p = startaddr;
    while(p <= startaddr + testsize){
        *p = 0x55;
        if(*p != 0x55){
            printf("Memory error\n");
        }
        p++;
        threadinfo->testpos++;
    }
}

void create_thread(){
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&info[threadnum].thread, &attr, memtest, &info[threadnum]);
    threadnum++;
}

int main(){
    int cmd;
    int i;
    for(i = 0; i < 10; i++){
        info[i].testpos = 0;
        info[i].testsize = 2000000;
        info[i].startaddr = (char*)malloc(info[i].testsize);
    }
    
    while(1){
        printf("Input(times(1),threads(2),go(3),status(4),abort(5),exit(6)): ");
        scanf("%d", &cmd);
        switch(cmd){
            case 1:
                break;
            case 2:
                break;
            case 3:
                create_thread();
                break;
            case 4:
                for(i = 0; i < threadnum; i++){
                    printf("Thread %d: %d/%d\n", i, info[i].testpos, info[i].testsize);
                }
                break;
            case 5:
                break;
            case 6:
                return 0;
                break;
        }
    }
    return 0;
}