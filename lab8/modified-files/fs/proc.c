#include <linux/proc.h>


int psinfo(char* buf, char* tmp){
    struct task_struct **p;
    int n = 0;
    n = sprintf(tmp,"pid\tstate\tfather\tcounter\tstart_time\n");
    for(p = &FIRST_TASK; p <= &LAST_TASK; p++){
        if(*p){
            n += sprintf((tmp+n),"%ld\t%ld\t%ld\t%ld\t%ld\n",(*p)->pid,(*p)->state,(*p)->father,(*p)->counter,(*p)->start_time);
        }
    }
    return n;
}

int hdinfo(char* buf, char* tmp){
    struct super_block *sb;
    sb = get_super(ROOT_DEV);
    struct buffer_head *bh;
    int n = 0;
    unsigned short total_blocks = sb->s_nzones;
    unsigned short total_inodes = sb->s_ninodes;
    unsigned short s_zmap_blocks = sb->s_zmap_blocks;
    unsigned short free_blocks = 0;
    unsigned short used_blocks = 0;
    int i = 0;
    for(i = 0; i < s_zmap_blocks; i++){
        bh = sb->s_zmap[i];
        char* p = bh->b_data;
        int j = 0;
        for(j = 0; j < 1024; j++){
            if(*(p+j) == 0){
                free_blocks++;
            }else{
                used_blocks++;
            }
        }
    }
    n += sprintf(tmp,"total_blocks: %d\n",total_blocks);
    n += sprintf(tmp+n,"total_inodes: %d\n",total_inodes);
    n += sprintf(tmp+n,"free_blocks: %d\n",free_blocks);
    n += sprintf(tmp+n,"used_blocks: %d\n",used_blocks);
    return n;
}

int proc_handler(int dev,char* buf,int count,off_t* f_pos){
    char* tmp;
    char* f_p;
    int n = 0;

    if(count > PAGE_SIZE) return 0;
    
    tmp = malloc(PAGE_SIZE);
    f_p = tmp + *f_pos;
    if(dev == 0){
        n = psinfo(buf,tmp);
    }else if(dev == 1){
        n = hdinfo(buf,tmp);
    }
    n -= *f_pos;
    n = count = n > count? count : n;
    while(n--){
        put_fs_byte(*(f_p++),buf++);
        (*f_pos)++;
    }
    free(tmp);
    return count;
}
