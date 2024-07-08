#ifndef PTHREAD_H
#define PTHREAD_H

typedef int size_t;
typedef unsigned long pthread_t;
#define StackSize 1024

typedef struct {
    void* stackaddr;
    int stacksize;
}pthread_attr_t;

int pthread_attr_init(pthread_attr_t* attr);
int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg);
int pthread_join(pthread_t thread, void** value_ptr);
int pthread_exit(void* value_ptr);

int pthread_attr_setstackaddr(pthread_attr_t* attr, void* stackaddr);
int pthread_attr_setstacksize(pthread_attr_t* attr, size_t* stacksize);


#endif