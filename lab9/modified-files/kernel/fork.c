/*
 *  linux/kernel/fork.c
 *
 *  (C) 1991  Linus Torvalds
 */

/*
 *  'fork.c' contains the help-routines for the 'fork' system call
 * (see also system_call.s), and some misc functions ('verify_area').
 * Fork is rather simple, once you get the hang of it, but the memory
 * management can be a bitch. See 'mm/mm.c': 'copy_page_tables()'
 */
#include <errno.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>

extern void write_verify(unsigned long address);

long last_pid=0;
long last_tid=0;

void verify_area(void * addr,int size)
{
	unsigned long start;

	start = (unsigned long) addr;
	size += start & 0xfff;
	start &= 0xfffff000;
	start += get_base(current->ldt[2]);
	while (size>0) {
		size -= 4096;
		write_verify(start);
		start += 4096;
	}
}

int copy_mem(int nr,struct task_struct * p)
{
	unsigned long old_data_base,new_data_base,data_limit;
	unsigned long old_code_base,new_code_base,code_limit;

	code_limit=get_limit(0x0f);
	data_limit=get_limit(0x17);
	old_code_base = get_base(current->ldt[1]);
	old_data_base = get_base(current->ldt[2]);
	if (old_data_base != old_code_base)
		panic("We don't support separate I&D");
	if (data_limit < code_limit)
		panic("Bad data_limit");
	new_data_base = new_code_base = nr * 0x4000000;
	p->start_code = new_code_base;
	set_base(p->ldt[1],new_code_base);
	set_base(p->ldt[2],new_data_base);
	if (copy_page_tables(old_data_base,new_data_base,data_limit)) {
		printk("free_page_tables: from copy_mem\n");
		free_page_tables(new_data_base,data_limit);
		return -ENOMEM;
	}
	return 0;
}

/*
 *  Ok, this is the main fork-routine. It copies the system process
 * information (task[nr]) and sets up the necessary registers. It
 * also copies the data segment in it's entirety.
 */
int copy_process(int nr,int nt,long ebp,long edi,long esi,long gs,long none,
		long ebx,long ecx,long edx,
		long fs,long es,long ds,
		long eip,long cs,long eflags,long esp,long ss)
{
	struct task_struct *p;
	struct thread_struct *q;
	int i;
	struct file *f;

	p = (struct task_struct *) get_free_page();
	q = (struct thread_struct *) get_free_page();
	if (!p || !q)
		return -EAGAIN;
	task[nr] = p;
	thread[nt] = q;
	*p = *current;	/* NOTE! this doesn't copy the supervisor stack */
	*q = *(current->main_thread);
	p->pid = last_pid;
	p->father = current->pid;
	p->signal = 0;
	p->alarm = 0;
	p->leader = 0;		/* process leadership doesn't inherit */
	p->utime = p->stime = 0;
	p->cutime = p->cstime = 0;
	p->start_time = jiffies;
	

	q->state = TASK_UNINTERRUPTIBLE;
	q->tid = last_tid;
	q->counter = q->priority;
	q->tss.back_link = 0;
	q->tss.esp0 = PAGE_SIZE + (long) q;
	q->tss.ss0 = 0x10;
	q->tss.eip = eip;
	q->tss.eflags = eflags;
	q->tss.eax = 0;
	q->tss.ecx = ecx;
	q->tss.edx = edx;
	q->tss.ebx = ebx;
	q->tss.esp = esp;
	q->tss.ebp = ebp;
	q->tss.esi = esi;
	q->tss.edi = edi;
	q->tss.es = es & 0xffff;
	q->tss.cs = cs & 0xffff;
	q->tss.ss = ss & 0xffff;
	q->tss.ds = ds & 0xffff;
	q->tss.fs = fs & 0xffff;
	q->tss.gs = gs & 0xffff;
	q->tss.ldt = _LDT(nt);
	q->tss.trace_bitmap = 0x80000000;

	p->main_thread = q;
	q->task = p;
	
	if (last_task_used_math == current)
		__asm__("clts ; fnsave %0"::"m" (p->main_thread->tss.i387));
	if (copy_mem(nr,p)) {
		task[nr] = NULL;
		free_page((long) p);
		return -EAGAIN;
	}
	for (i=0; i<NR_OPEN;i++)
		if ((f=p->filp[i]))
			f->f_count++;
	if (current->pwd)
		current->pwd->i_count++;
	if (current->root)
		current->root->i_count++;
	if (current->executable)
		current->executable->i_count++;
	set_tss_desc(gdt+(nt<<1)+FIRST_TSS_ENTRY,&(q->tss));
	set_ldt_desc(gdt+(nt<<1)+FIRST_LDT_ENTRY,&(p->ldt));
	q->state = TASK_RUNNING;	/* do this last, just in case */
	return last_pid;
}

int find_empty_process(void)
{
	int i;

	repeat:
		if ((++last_pid)<0) last_pid=1;
		for(i=0 ; i<NR_TASKS ; i++)
			if (task[i] && task[i]->pid == last_pid) goto repeat;
	for(i=1 ; i<NR_TASKS ; i++)
		if (!task[i])
			return i;
	return -EAGAIN;
}

int find_empty_thread(void)
{
	int i;

	repeat:
		if ((++last_tid)<0) last_tid=1;
		for(i=0 ; i<NR_THREADS; i++)
			if (thread[i] && thread[i]->tid == last_tid) goto repeat;
	for(i=1 ; i<NR_THREADS ; i++)
		if (!thread[i]) {
			return i;
		}
	return -EAGAIN;
}
