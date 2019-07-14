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
#include <kern/trap.h>
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
	{"time", "Displat the program's running time", mon_time},
	{"showmappings","Display the permission bits to the pages at virtual addresses",showmappings},
	{"setperm","Set permission bit at addr",setperm},
	{"clearperm","Clear permission bit at addr",clearperm},	
	{"dumppa","Show raw data between 2 physical address",dumppa},
	{"dumpva","Show raw data between 2 virtual adderss",dumpva},
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
int mon_time(int argc, char **argv, struct Trapframe *tf)
{
	if(argc==1)
	{
		cprintf("Usage: time [command]\n");
		return 0;
	}
	int i=0;
	while(i<sizeof(commands)/sizeof(commands[0]))
	{
		//first find the command
		if(strcmp(argv[1],commands[i].name)==0)
		{
			unsigned long long time1=read_tsc();
			commands[i].func(argc-1,argv+1,tf);
			unsigned long long time2=read_tsc();
			cprintf("%s cycles: %llu\n",argv[1],time2-time1);
			return 0;
		}
		i++;
	}
	cprintf("Unknown command\n",argv[1]);
	return 0;
}

//challenge help funcs

//get a char* like "0x50000", return int value
//char * should start with 0x
uint32_t char2int(char *num)
{
	assert(num[0]=='0'&&num[1]=='x');
	uint32_t res = 0;
	num +=2;
	while(*num!='\0')
	{
		if (*num >= 'a') *num = *num-'a'+'0'+10;
		res = res*16 + *num - '0';
		num++;
	}
	return res;
}

void print_pte(pte_t *pte)
{
	cprintf("PTE_P: %x, PTE_W: %x, PTE_U: %x\n", *pte&PTE_P, *pte&PTE_W, *pte&PTE_U);
}

int showmappings(int argc,char **argv, struct Trapframe *tf)
{
	if(argc == 1)
	{
		cprintf("Usage: eg showmappings  0x4000000, 0x5000000\n");
		return 0;
	}
	uint32_t s_addr=char2int(argv[1]);
	uint32_t e_addr=char2int(argv[2]);
	if(s_addr>e_addr)
	{
		cprintf("Invalid showmappings: start addr should be lower\n");
		return 0;
	}
	cprintf("mappings from 0x%x to 0x%x\n",s_addr,e_addr);
	//todo show the pa ???
	while(s_addr<e_addr){
		//create pt
		pte_t *tmp = pgdir_walk(kern_pgdir, (void *) s_addr, 1);
		if(tmp==NULL)
		{
			panic("showmapping panic,out of memory\n");
		}
		if(*tmp&PTE_P)
		{
			cprintf("page 0x%x, perm bit:",s_addr);
			print_pte(tmp);			
		}
		else{
			cprintf("page 0x%x not PTE_P\n",s_addr);
		}
		s_addr+=PGSIZE;
	}
	return 0;
}

//set perm bit of a va
int setperm(int argc, char **argv, struct Trapframe *tf)
{
	if(argc == 1)
	{
		cprintf("Usage: setperm addr permissions, eg setperm 0x3000 P\n");
		return 0;
	}
	uint32_t target=char2int(argv[1]);
	pte_t *tmp = pgdir_walk(kern_pgdir,(void *)target,1);
	if(tmp==NULL)
	{
		panic("setperm panic,out of memory");
	}
	cprintf("0x%x set permission(P|W|U) from: %x|%x|%x ",
		target,*tmp&PTE_P, *tmp&PTE_W, *tmp&PTE_U);
	uint32_t new_perm = 0;
	if(argv[2][0]=='P')new_perm=PTE_P;
	else if(argv[2][0]=='W') new_perm=PTE_W;
	else if(argv[2][0]=='U') new_perm=PTE_U;
	*tmp = *tmp|new_perm;
	cprintf("to %x|%x|%x\n",*tmp&PTE_P, *tmp&PTE_W, *tmp&PTE_U);
	return 0;
}

int clearperm(int argc, char **argv, struct Trapframe *tf)
{
	if(argc == 1)
	{
		cprintf("Usage: clearperm addr permissions, eg setperm 0x3000 P\n");
		return 0;
	}
	uint32_t target=char2int(argv[1]);
	pte_t *tmp = pgdir_walk(kern_pgdir,(void *)target,1);
	if(tmp==NULL)
	{
		panic("clearperm panic,out of memory");
	}
	cprintf("0x%x clear permission(P|W|U) from: %x|%x|%x ",
		target,*tmp&PTE_P, *tmp&PTE_W, *tmp&PTE_U);
	uint32_t new_perm = 0;
	if(argv[2][0]=='P')new_perm=PTE_P;
	else if(argv[2][0]=='W') new_perm=PTE_W;
	else if(argv[2][0]=='U') new_perm=PTE_U;
	*tmp = *tmp&~new_perm;
	cprintf("to %x|%x|%x\n",*tmp&PTE_P, *tmp&PTE_W, *tmp&PTE_U);
	return 0;
}

int dumppa(int argc, char **argv, struct Trapframe *tf)
{
	if(argc == 1)
	{
		cprintf("Usage: dumppa start_addr end_addr\n");
	}
	void* s_addr=(void*)char2int(argv[1]);
	void* e_addr=(void*)char2int(argv[2]);
	cprintf("dump from pa 0x%x to 0x%x\n",s_addr,e_addr);

	for(;s_addr<e_addr;s_addr++)
	{
		cprintf("%08x ",*(char*)KADDR((uint32_t)s_addr));
	}
	cprintf("\n");
	return 0;

}

int dumpva(int argc, char **argv, struct Trapframe *tf)
{
	if(argc == 1)
	{
		cprintf("Usage: dumpva start_addr end_addr\n");
	}
	void* s_addr=(void*)char2int(argv[1]);
	void* e_addr=(void*)char2int(argv[2]);
	cprintf("dump from va 0x%x to 0x%x\n",s_addr,e_addr);

	for(;s_addr<e_addr;s_addr++)
	{
		cprintf("%08x ",*(char*)(s_addr));
	}
	cprintf("\n");
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
    char *pret_addr;

	// Your code here.
    pret_addr = (char *)read_pretaddr();
	uint32_t new_addr = (uint32_t)do_overflow;

	/*
	|args   |		|old ret|
	|old ret|  -> 	|new ret|
	|-------|		|-------|
	*/
	//back up old ret addr first
	for (int i = 0; i < 4; i++){
		cprintf("%*s%n\n", pret_addr[i] & 0xFF, " ", pret_addr + 4 + i);
	}
    //overwrite new ret_addr
	for (int i = 0; i < 4; i++){
		cprintf("%*s%n\n", (new_addr >> (8*i)) & 0xFF, " ", pret_addr + i);
	}

}

void
overflow_me(void)
{
        start_overflow();
}


int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// Your code here.

	uint32_t ebp = read_ebp();// indicate the base pointer into the stack used by that function
	uint32_t esp = read_esp();// the instruction address to which control will return when the function return

	cprintf("Stack backtrace:\n");
	while(ebp!=0)
	{	
		/*
			when call a func, push args from right to left
			push ret addr
			enter func
			mov esp, ebp
			push ebp
		*/
		//get args esp from stack
		esp = *((uint32_t *)ebp + 1);
		uint32_t arg1 = *((uint32_t *)ebp + 2);
		uint32_t arg2 = *((uint32_t *)ebp + 3);
		uint32_t arg3 = *((uint32_t *)ebp + 4);
		uint32_t arg4 = *((uint32_t *)ebp + 5);
		uint32_t arg5 = *((uint32_t *)ebp + 6);

		cprintf("  eip %08x ebp %08x args %08x %08x %08x %08x %08x\n",esp,ebp,arg1,arg2,arg3,arg4,arg5);
		//output the line no
		struct Eipdebuginfo info;
		debuginfo_eip((uintptr_t)esp, &info);
		//get func name
		char func_name[info.eip_fn_namelen+1];
		for(int i=0;i<info.eip_fn_namelen;i++){
			func_name[i]=info.eip_fn_name[i];
		}
		func_name[info.eip_fn_namelen]='\0';
		uint32_t lineno = esp - (uint32_t)info.eip_fn_addr;
		cprintf("         %s:%u %s+%u\n",info.eip_file, info.eip_line, func_name, lineno);
		ebp = *((uint32_t*)ebp);
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

	if (tf != NULL)
		print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
