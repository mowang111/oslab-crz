#ifndef PTHREAD_H
#define PTHREAD_H

#define __LIBRARY__
#include <unistd.h>
#include <linux/pckernel.h>

#define PARASIZE 4

int pthread_attr_init(pthread_attr_t* attr);
int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* start_routine, void* arg);
int pthread_join(pthread_t thread, void** value_ptr);
int pthread_exit(void* value_ptr);

int pthread_attr_setstackaddr(pthread_attr_t* attr, void* stackaddr);
int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stacksize);


#endif