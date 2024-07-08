#ifndef PROC_H
#define PROC_H

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <sys/types.h>
#include <asm/segment.h>
#include <linux/fs.h>

int proc_handler(int dev, char* buf, int count, off_t* f_pos);


#endif