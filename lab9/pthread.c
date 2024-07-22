#include "pthread.h"

_syscall3(int, pthread_create_kernel, pthread_t*, thread, const pthread_attr_t*, attr, void*, start_routine);

int pthread_attr_setstackaddr(pthread_attr_t* attr, void* stackaddr) {
    attr->stackaddr = stackaddr;
    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stacksize) {
    attr->stacksize = stacksize;
    return 0;
}

int pthread_attr_init(pthread_attr_t* attr) {
    void* addr  = malloc(StackSize);
    pthread_attr_setstackaddr(attr, addr);
    pthread_attr_setstacksize(attr, StackSize);
    return 0;
}

int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* start_routine, void* arg){
    long* addr = attr->stackaddr - PARASIZE;
    *addr = arg;
    attr->stackaddr = attr->stackaddr - PARASIZE - 4;
    printf("start_routine address %#x\n", start_routine);
    pthread_create_kernel(thread, attr, start_routine);
    return 0;
}
