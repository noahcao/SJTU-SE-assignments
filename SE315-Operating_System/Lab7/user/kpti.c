// hello, world
#include <inc/lib.h>

static void
check_isolate()
{
	uint32_t cnt = 0;
	for (uintptr_t va = KERNBASE; va; va += PGSIZE) {
		if (uvpd[PDX(va)] & PTE_P) {
			if (uvpd[PDX(va)] & PTE_PS)
				cprintf("user mapped %08x\n", va);
			else {
				if (uvpt[PGNUM(va)] & PTE_P)
					cprintf("user mapped %08x\n", va);
			}
		}
	}
}

extern char s_in_text[];
char s_in_data[] = "string in .data";
char s_in_page_map[] __attribute__((aligned(PGSIZE))) = "string in sys_page_map";
char s_in_ipc_recv[] __attribute__((aligned(PGSIZE))) = "string in ipc recv";

static void
check_consistent()
{
	asm volatile(
		"jmp 1f\n"
		"s_in_text:\n"
		".ascii \"string in .text\\0\"\n"
		"1:\n"
		:::"memory");
	cprintf("%s\n", s_in_text);
	cprintf("%s\n", "string in .rodata");
	cprintf("%s\n", s_in_data);
	assert(sys_page_alloc(0, UTEMP, PTE_W | PTE_P | PTE_U) == 0);
	char *s_in_page_alloc = (char *) UTEMP;
	strcpy(s_in_page_alloc, "string in page alloc");
	cprintf("%s\n", s_in_page_alloc);
	assert(sys_page_unmap(0, UTEMP) == 0);
	assert(sys_page_map(0, s_in_page_map, 0, UTEMP, PTE_U | PTE_P) == 0);
	cprintf("%s\n", UTEMP);
	envid_t child;
	child = fork();
	assert(child >= 0);
	if (child == 0) {
		assert(ipc_recv(NULL, UTEMP, NULL) == 1234);
		cprintf("%s\n", UTEMP);
		exit();
	}
	ipc_send(child, 1234, s_in_ipc_recv, PTE_U | PTE_P);
	wait(child);
}

static void
check_memory_leak()
{
	uintptr_t end;
	for (uintptr_t va = PTSIZE * 4; ; va += PGSIZE) {
		if (sys_page_alloc(0, (void *) va, PTE_U | PTE_P) == -E_NO_MEM) {
			end = va;
			break;
		}
	}
	for (uintptr_t va = PTSIZE * 4; va < PTSIZE * 4 + 10 * PGSIZE; va += PGSIZE)
		assert(sys_page_unmap(0, (void *) va) == 0);
	for (int i = 0; i < 1000; i++)
		assert(sys_page_alloc(0, (void *) (PTSIZE * 4), PTE_U | PTE_P) == 0);
	for (uintptr_t va = PTSIZE * 4; va < end; va += PGSIZE)
		assert(sys_page_unmap(0, (void *) va) == 0);
}

static void
check_memory_leak2()
{
	uintptr_t end;
	for (uintptr_t va = PTSIZE * 4; ; va += PGSIZE) {
		if (sys_page_alloc(0, (void *) va, PTE_U | PTE_P) == -E_NO_MEM) {
			end = va;
			break;
		}
	}
	assert(end - PTSIZE * 4 > PGSIZE * 20);
	for (uintptr_t va = PTSIZE * 4; va < end; va += PGSIZE)
		assert(sys_page_unmap(0, (void *) va) == 0);
	end -= PGSIZE * 20;
	for (int i = 0; i < 20; i++) {
		envid_t child;
		child = fork();
		assert(child >= 0);
		if (child == 0) {
			for (uintptr_t va = PTSIZE * 4; va < end; va += PGSIZE)
				assert(sys_page_alloc(0, (void *) va, PTE_U | PTE_P) == 0);
			exit();
		}
		wait(child);
	}
}

void
umain(int argc, char **argv)
{
	envid_t child;
	child = fork();
	assert(child >= 0);
	if (child == 0) {
		sys_env_set_pgfault_upcall(0, NULL);
		cprintf("%x\n", *(volatile uint8_t *) 0xf0000000);
		return;
	}
	wait(child);
	cprintf("kernel space isolated\n");
	check_consistent();
	cprintf("consistent env_kern_pgdir\n");
	check_memory_leak();
	check_memory_leak2();
	cprintf("no memory leak\n");
}
