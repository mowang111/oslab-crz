#include <linux/pckernel.h>
#include <asm/segment.h>
#include <asm/system.h>

pthread_attr_t* get_attr_from_user(const pthread_attr_t* attr){
    pthread_attr_t* attr_kernel = (pthread_attr_t*)malloc(sizeof(pthread_attr_t));
    attr_kernel->stackaddr = get_fs_long(attr);
    attr_kernel->stacksize = get_fs_word(attr+sizeof(void*));
    return attr_kernel;
}

int sys_pthread_create_kernel(pthread_t* thd, const pthread_attr_t* attr, void* start_routine){
    struct thread_struct* q = (struct thread_struct*) get_free_page();
    struct thread_struct* main_thread = current->main_thread;
    int nt = find_empty_thread();
    if (!q || nt < 0) {
        return -1;
    }
    if (nt >= NR_THREADS) {
        printk("Thread index out of bounds: %d\n", nt);
        free_page((unsigned long)q);
        return -1;
    }

    q->state = TASK_UNINTERRUPTIBLE;
	q->tid = last_tid;
    q->task = current;
	q->priority = main_thread->priority;
	q->counter = q->priority;
    q->tss.back_link = 0;
	q->tss.esp0 = PAGE_SIZE + (long) q;
    q->tss.esp = get_attr_from_user(attr)->stackaddr;
	q->tss.eflags = main_thread->tss.eflags;
	q->tss.eax = 0;
    q->tss.ss0 = 0x10;
    q->tss.cs = 0x0f;
    q->tss.eip = start_routine;
	q->tss.ecx = main_thread->tss.ecx;
	q->tss.edx = main_thread->tss.edx;
	q->tss.ebx = main_thread->tss.ebx;
	q->tss.ebp = main_thread->tss.ebp;
	q->tss.esi = main_thread->tss.esi;
	q->tss.edi = main_thread->tss.edi;
    q->tss.ss = main_thread->tss.ss & 0xffff;
	q->tss.es = main_thread->tss.es & 0xffff;
	q->tss.ds = main_thread->tss.ds & 0xffff;
	q->tss.fs = main_thread->tss.fs & 0xffff;
	q->tss.gs = main_thread->tss.gs & 0xffff;
	q->tss.ldt = _LDT(nt);
    q->tss.trace_bitmap = 0x80000000;
    q->tss.i387 = main_thread->tss.i387;
    set_tss_desc(gdt+(nt<<1)+FIRST_TSS_ENTRY,&(q->tss));
	set_ldt_desc(gdt+(nt<<1)+FIRST_LDT_ENTRY,&(current->ldt));
    put_fs_long(q->tid,thd);
    q->state = TASK_RUNNING;
    thread[nt] = q;
    return 0;
}