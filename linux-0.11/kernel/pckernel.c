#include <linux/pckernel.h>
#include <linux/sched.h>

int sys_pthread_create_kernel(pthread_t* thread, const pthread_attr_t* attr, void* start_routine){
    struct thread_struct* q = (struct thread_struct*) get_free_page();
    int nt = find_empty_thread();
    if (!q || nt < 0) {
        return -1;
    }
	q->tid = last_tid;
    q->task = current;
	q->counter = q->priority;
	q->tss.esp0 = PAGE_SIZE + (long) q;
    q->tss.esp = attr->stackaddr;
	q->tss.ss0 = 0x10;
    q->tss.ss = 0x17;
	q->tss.eflags = 0x200; /* IF = 1; */
	q->tss.eax = 0;
    q->tss.eip = (long) start_routine;
	q->tss.ldt = _LDT(nt);
    q->state = TASK_RUNNING;

    *thread = q->tid;
    thread[nt] = q;
    return 0;
}