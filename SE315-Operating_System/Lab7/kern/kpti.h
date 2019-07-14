#ifndef JOS_INC_KPTI_H
#define JOS_INC_KPTI_H

#include <kern/cpu.h>

#define __user_mapped_text __attribute__((section(".user_mapped.text")))
#define __user_mapped_data __attribute__((section(".user_mapped.data")))

extern char __USER_MAP_BEGIN__[]; // Beginning of user mapped kernel page
extern char __USER_MAP_END__[]; // End of user mapped kernel page
extern pde_t *cur_kern_pgdir[NCPU];

#endif
