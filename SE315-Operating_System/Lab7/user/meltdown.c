// hello, world
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	uint32_t cnt = 0;
	for (uintptr_t va = ULIM; va; va += PGSIZE) {
		if ((uvpd[PDX(va)] & PTE_P) && ((uvpd[PDX(va)] & PTE_PS) || (uvpt[PGNUM(va)] & PTE_P)))
			cnt++;
	}
	cprintf("%u kernel pages has PTE_P\n", cnt);
}
