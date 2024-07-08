#define __LIBRARY__
#include <unistd.h>
#ifndef _LINUX_SHM_H
#define _LINUX_SHM_H
#include <sys/types.h>
#include <linux/sched.h>
#include <linux/mm.h>

typedef int key_t;
typedef struct shmid_ds shmid_ds;

#define MAX_SHM_NUM 32

/*shmid map physic address */
struct shmid_ds {
    key_t key;
    int shmid;
    int size;
    unsigned long addr;
    int is_used;
};

shmid_ds shm_table[MAX_SHM_NUM];

_syscall3(int, shmget, key_t, key, size_t, size, int, shmflg);
_syscall3(void *, shmat, int, shmid, unsigned long, addr, int, shmflg);

#endif