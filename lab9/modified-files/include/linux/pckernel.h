#ifndef PC_KERNEL_H
#define PC_KERNEL_H

#include <linux/sched.h>

typedef unsigned long pthread_t;
#define StackSize 1024
extern long last_tid;

typedef struct {
    void* stackaddr;
    int stacksize;
}pthread_attr_t;

extern struct thread_struct *thread[NR_THREADS];
int sys_pthread_create_kernel(pthread_t* thread, const pthread_attr_t* attr, void* start_routine);


#endif