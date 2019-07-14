// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/pmap.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
    {"showmappings","Display in a useful and easy-to-read format all of the physical page mappings",showmappings},
    {"setpermbit","Set permission bit at target addr",setpermbit},
    {"dump","Show raw data between 2 physical/visual address",dump},
};

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(commands); i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

// Lab1 only
// read the pointer to the retaddr on the stack
static uint32_t
read_pretaddr() {
    uint32_t pretaddr;
    __asm __volatile("leal 4(%%ebp), %0" : "=r" (pretaddr)); 
    return pretaddr;
}

void
do_overflow(void)
{
    cprintf("Overflow success\n");
}

void
start_overflow(void)
{
	// You should use a techique similar to buffer overflow
	// to invoke the do_overflow function and
	// the procedure must return normally.

    // And you must use the "cprintf" function with %n specifier
    // you augmented in the "Exercise 9" to do this job.

    // hint: You can use the read_pretaddr function to retrieve 
    //       the pointer to the function call return address;

    char str[256] = {};
    int nstr = 0;
    char *pret_add;

	// Your code here.
    pret_add = (char*) read_pretaddr();

    memset(str,0,256);
    cprintf("%x \n",*pret_add);

	cprintf("%25d%n\n",1,pret_add);
	cprintf("%9d%n\n",1,pret_add+1);
	cprintf("%16d%n\n",1,pret_add + 2);

    return;
}

void
overflow_me(void){
        start_overflow();
        cprintf("\n");
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// Your code here.
    unsigned int *ebp = (unsigned int*)read_ebp();
    struct Eipdebuginfo debug_info;
	unsigned int eip = ebp[1]; // Upper level eip.
	while (ebp != NULL) {
		cprintf("  eip %08x  ebp %08x  args %08x %08x %08x %08x %08x\n", eip, ebp, ebp[2], ebp[3], ebp[4], ebp[5], ebp[6]);
		debuginfo_eip(eip, &debug_info);
		cprintf("	 %s:%d ", debug_info.eip_file, debug_info.eip_line);
		for(int i = 0; i < debug_info.eip_fn_namelen; i++) 
			cprintf("%c", debug_info.eip_fn_name[i]);
		cprintf("+%d\n", eip - debug_info.eip_fn_addr);
		ebp = (unsigned int *)ebp[0]; 
		eip = ebp[1]; 
	}

	overflow_me();
    cprintf("Backtrace success\n");
	return 0;
}



/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");


	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}

// ############## Challenge 2 in Lab2 ###################
int showmappings(int argc, char** argv, struct Trapframe *tf){
    // the legal input format should be :
    //      showmappings va_adrr1 va_adrr2
    uint32_t va1, va2;
    uint32_t va = 0;

    bool format_error = false;
    if(argc != 3) format_error = true;
    va1 = strtol(argv[1], NULL, 16);
    va2 = strtol(argv[2], NULL, 16);
    if(!va1 || !va2) format_error = true;

    bool va1_aligned = (va1 == ROUNDUP(va1, PGSIZE));
    bool va2_aligned = (va2 == ROUNDUP(va2, PGSIZE));

    if(!va1_aligned || !va2_aligned || va1>va2 || va1==va2) format_error = true;

    if(format_error){
        cprintf("Not expected input!\n");
        return 0;
    }
    else{
        while(va1 < va2){
            pte_t *temp_pte = pgdir_walk(kern_pgdir, (void*)va1, 1);
            if(!temp_pte) panic("memory to showmappings is out of memory\n");
            if(*temp_pte & PTE_P){
                cprintf("page 0x%x, perm bit is set\n", va1);
            }
            else{
                cprintf("page x%x, perm bit is not set\n", va1);
            }
            va1 += PGSIZE;
        }
    }
    return 0;
}

int setpermbit(int argc, char **argv, struct Trapframe *tf){
    uint32_t va = 0;
    uint32_t perm = 0;

    char type, flag;
    bool format_error = false;

    if(argc != 4) format_error = true;
    if(!(va = strtol(argv[1], NULL, 16))) format_error = true;

    type = argv[2][0];
    flag = argv[3][0];

    bool va_aligned = (va == ROUNDUP(va, PGSIZE));
    bool type_legal = type == 'c' || type == 's';
    if(!va_aligned || !type_legal) format_error = true;

    switch(flag){
        case 'P': perm = PTE_P; break;
        case 'W': perm = PTE_W; break;
        case 'U': perm = PTE_U; break;
        default: format_error = true;
    }

    if(format_error){
        cprintf("Not expected input! Usage\n");
        cprintf(" > permission 0xva [c|s :clear or set] [P|W|U]\n");
        return 0;
    }

    pte_t *pte = pgdir_walk(kern_pgdir, (void*)va, 1);
    cprintf("origin:  0x%08x\tP: %1d\tW: %1d\tU: %1d\n", va, *pte&PTE_P, (*pte&PTE_W)>>1, (*pte&PTE_U)>>2);

    if(type == 'c'){
        cprintf("clear the target perm bit at 0x%x\n", va);
        *pte = *pte & ~perm;
    }
    else{
        cprintf("setting the target perm bit at 0x%x\n", va);
        *pte = *pte | perm;
    }

    return 0;
}

int dump(int argc, char **argv, struct Trapframe *tf)
{
	if(argc != 4){
        cprintf("Usage: dumppa [v|p] start_addr end_addr\n");
        return 0;
    }

    char flag = argv[1][0];
    if(flag != 'v' && flag != 'p'){
        cprintf("Usage: dumppa [v|p] start_addr end_addr\n");
        return 0;
    }
	uint32_t va1 = strtol(argv[2], NULL, 16);
	uint32_t va2 = strtol(argv[3], NULL, 16);

    if(flag == 'v'){
        // dump the visual memory
        cprintf("dump from va 0x%x to 0x%x\n",va1,va2);

	    for(; va1 < va2; va1 += 4) cprintf("0x%08x: %08x \n",va1, *(char*)(va1));
    }
    else{
        // dump the physical memory
        cprintf("dump from pa 0x%x to 0x%x\n", va1, va2);

        for(;va1 < va2; va1 += 4) cprintf("0x%08x: %08x \n",va1, *(char*)KADDR(va1));
    }
	return 0;
}
