#include <sys/shm.h>

int sys_shmget(key_t key, size_t size, int shmflg){
    int i;
    /*judge if size > a page*/
    if(size > PAGE_SIZE){
        return -1;
    }

    /*find if the key is already in the shm_table*/
    for(i = 0; i < MAX_SHM_NUM; i++){
        if(shm_table[i].key == key){
            add_memmap(shm_table[i].addr);
            return shm_table[i].shmid;
        }
    }

    /*find a free shmid*/
    for(i = 0; i < MAX_SHM_NUM; i++){
        if(!shm_table[i].is_used){
            break;
        }
    }

    shm_table[i].key = key;
    shm_table[i].shmid = i;
    shm_table[i].size = size;
    shm_table[i].addr = (int *)get_free_page();
    shm_table[i].is_used = 1;
    printk("shm key: %d, phyaddr: %ld\n", key, shm_table[i].addr);

    return i;
}

void * sys_shmat(int shmid, unsigned long addr, int shmflg){
    unsigned long tmp;
    if(shmid > MAX_SHM_NUM || !shm_table[shmid].is_used){
        return -1;
    }
    if(!addr){
        /*find a page virtual page from data segment*/
        tmp = get_base(current->ldt[1]) + current->brk;
        put_page(shm_table[shmid].addr, tmp);
        printk("phyadr: %lu has been maped to virsual adr: %lu\n", shm_table[shmid].addr, tmp);
        return current->brk;
    }
    return NULL;
}
