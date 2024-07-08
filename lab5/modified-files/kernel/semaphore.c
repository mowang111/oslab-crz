#include <semaphore.h>
#include <asm/system.h>
#include <asm/segment.h>
#include <linux/kernel.h>

struct sem_t* sys_sem_open(const char *name, unsigned int value) {
    // assign num for sem
    int i = 0, min_i = MAX_SEM_NUM;
    int name_len = 0;
    char c;
    char tmp_name[SEM_NAME_LEN];

    while(c = get_fs_byte(name++)){
        tmp_name[name_len] = c;
        name_len++;
    }
    tmp_name[name_len] = '\0';
    
    for(i = 0; i < MAX_SEM_NUM; i++){
        if(sem_bitmap & (1 << i)){
            if(!strcmp(tmp_name, sem_array[i].name)){
                sem_array[i].sem_cnt++;
                printk("sem_open sem name: %s has already exist, addr: %x\n", tmp_name, &sem_array[i]);
                return &sem_array[i];
            }   
        }else{
            min_i = i < min_i ? i : min_i;
        }
    }

    if(min_i == MAX_SEM_NUM){
        return NULL;
    }
    struct sem_t* sem = &sem_array[min_i];
    sem->name = malloc(SEM_NAME_LEN);
    strcpy(sem->name, tmp_name);
    sem->sem_value = value;
    sem->sem_wait = NULL;
    sem->sem_cnt++;
    sem_bitmap |= (1 << min_i);
    printk("sem_open sem name: %s, addr: %x\n", sem->name, sem);
    return sem;
}

int sys_sem_wait(struct sem_t* sem) {
    while(sem->sem_value <= 0){
        sleep_on(&sem->sem_wait);
    }
    cli();
    sem->sem_value--;
    sti();
    return 0;
}

int sys_sem_post(struct sem_t* sem) {
    cli();
    sem->sem_value++;
    sti();
    wake_up(&sem->sem_wait);
    return 0;
}

int sys_sem_unlink(const char *name) {
    char kname[SEM_NAME_LEN];
    int name_len = 0;
    char c;
    while(c = get_fs_byte(name++)){
        kname[name_len] = c;
        name_len++;
    }
    kname[name_len] = '\0';
    printk("sem_unlink name: %s\n", kname);
    int i = 0;
    for(i = 0; i < MAX_SEM_NUM; i++){
        if(!strcmp(sem_array[i].name, kname) && (sem_bitmap & (1 << i))){
            cli();
            sem_array[i].sem_cnt--;
            sti();
            if(!sem_array[i].sem_cnt){
                sem_bitmap &= ~(1 << i);
                printk("free: %s, sem_cnt: %d\n",sem_array[i].name, sem_array[i].sem_cnt);
                free(sem_array[i].name);
            }
            return 0;
        }
    }
    return -1;
}

