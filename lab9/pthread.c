#include "pthread.h"

int pthread_attr_setstackaddr(pthread_attr_t* attr, void* stackaddr) {
    attr->stackaddr = stackaddr;
}

int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stacksize) {
    attr->stacksize = StackSize;
}

int pthread_attr_init(pthread_attr_t* attr) {
    attr->stackaddr = NULL;
    attr->stacksize = StackSize;
}

int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg){
    long* addr = attr->stackaddr - PARASIZE;
    *addr = arg;
    attr->stackaddr = attr->stackaddr - PARASIZE - 4;
}
