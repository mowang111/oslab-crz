#define __LIBRARY__
#include <unistd.h>
#ifndef _LINUX_SEM_H
#define _LINUX_SEM_H

#include <linux/sched.h>

#define MAX_SEM_NUM 10
#define SEM_NAME_LEN 32

typedef struct sem_t sem_t;

struct sem_t {
    char * name;
    unsigned int sem_value;
    struct task_struct * sem_wait;
};

struct sem_t sem_array[MAX_SEM_NUM];
unsigned int sem_bitmap = 0;


_syscall2(sem_t*, sem_open, const char*, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t*, sem);
_syscall1(int, sem_post, sem_t*, sem);
_syscall1(int, sem_unlink, const char*, name);


#endif


