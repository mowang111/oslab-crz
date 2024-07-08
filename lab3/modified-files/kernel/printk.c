/*
 *  linux/kernel/printk.c
 *
 *  (C) 1991  Linus Torvalds
 */

/*
 * When in kernel-mode, we cannot use printf, as fs is liable to
 * point to 'interesting' things. Make a printf with fs-saving, and
 * all is well.
 */
// #define __LIBRARY__
#include <stdarg.h>
#include <stddef.h>
#include <linux/sched.h>

#include <linux/kernel.h>
// #include <unistd.h>


static char buf[1024];

extern int vsprintf(char * buf, const char * fmt, va_list args);
extern int file_write(struct m_inode * inode, struct file * filp,
		char * buf, int count);
// static inline _syscall0(int,sync);

int printk(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);
	__asm__("push %%fs\n\t"
		"push %%ds\n\t"
		"pop %%fs\n\t"
		"pushl %0\n\t"
		"pushl $buf\n\t"
		"pushl $0\n\t"
		"call tty_write\n\t"
		"addl $8,%%esp\n\t"
		"popl %0\n\t"
		"pop %%fs"
		::"r" (i):"ax","cx","dx");
	return i;
}

static char logbuf[1024];
int fprintk(int fd, const char *fmt, ...)
{
	va_list args;
	int count;
	struct file * file;
	struct m_inode * inode;
	va_start(args, fmt);
	count = vsprintf(logbuf, fmt, args);
	va_end(args);
	if (fd == 3) {
		// get file by 0 process
		if (!(file=task[0]->filp[fd]))    /* 从进程0的文件描述符表中得到文件句柄 */
            return 0;
		inode = file->f_inode;
		__asm__("push %%fs\n\t"
				"push %%ds\n\t"
				"pop %%fs\n\t"
				"pushl %0\n\t"
				"pushl $logbuf\n\t"
				"pushl %1\n\t"
				"pushl %2\n\t"
				"call file_write\n\t"
				"addl $12,%%esp\n\t"
				"popl %0\n\t"
				"pop %%fs\n\t"
				::"r"(count), "r"(file), "r"(inode):"ax","cx","dx");
	}
	
	return count;
}