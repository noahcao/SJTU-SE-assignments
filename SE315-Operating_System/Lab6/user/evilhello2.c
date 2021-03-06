// evil hello world -- kernel pointer passed to kernel
// kernel should destroy user environment in response

#include <inc/lib.h>
#include <inc/mmu.h>
#include <inc/x86.h>

char vaddr[PGSIZE];
struct Segdesc old;
struct Segdesc *gdt;
struct Segdesc *entry;

char user_gdt[PGSIZE*2];
struct Segdesc *gdte_ptr,gdte_backup;
static void (*ring0_call_func)(void) = NULL;

// Call this function with ring0 privilege
void evil()
{
	// Kernel memory access
	*(char*)0xf010000a = 0;

	// Out put something via outb
	outb(0x3f8, 'I');
	outb(0x3f8, 'N');
	outb(0x3f8, ' ');
	outb(0x3f8, 'R');
	outb(0x3f8, 'I');
	outb(0x3f8, 'N');
	outb(0x3f8, 'G');
	outb(0x3f8, '0');
	outb(0x3f8, '!');
	outb(0x3f8, '!');
	outb(0x3f8, '!');
	outb(0x3f8, '\n');
}

void call_fun_ptr(){
    evil();  
    *entry = old;  
    asm volatile("popl %ebp");
    asm volatile("lret");   
}

static void
sgdt(struct Pseudodesc* gdtd){
	__asm __volatile("sgdt %0" :  "=m" (*gdtd));
}

static void
call_fun_wrapper(){
    ring0_call_func();
    *gdte_ptr = gdte_backup;
    asm volatile("leave");
    asm volatile("lret");
}


// Invoke a given function pointer with ring0 privilege, then return to ring3
void ring0_call(void (*fun_ptr)(void)) {
    // Here's some hints on how to achieve this.
    // 1. Store the GDT descripter to memory (sgdt instruction)
    // 2. Map GDT in user space (sys_map_kernel_page)
    // 3. Setup a CALLGATE in GDT (SETCALLGATE macro)
    // 4. Enter ring0 (lcall instruction)
    // 5. Call the function pointer
    // 6. Recover GDT entry modified in step 3 (if any)
    // 7. Leave ring0 (lret instruction)

    // Hint : use a wrapper function to call fun_ptr. Feel free
    //        to add any functions or global variables in this 
    //        file if necessary.

    // Lab3 : Your Code Here
    struct Pseudodesc gdtd;
    int GD_EVIL = GD_UD; 

    sgdt(&gdtd);
    int r;
    if((r = sys_map_kernel_page((void* )gdtd.pd_base, (void* )user_gdt)) < 0){
      cprintf("ring0_call: sys_map_kernel_page failed, %e\n", r);
      return ;
    }
    ring0_call_func = fun_ptr;
    struct Segdesc *gdt = (struct Segdesc*)((uint32_t)(PGNUM(user_gdt) << PTXSHIFT) + PGOFF(gdtd.pd_base));
    gdte_backup = *(gdte_ptr = &gdt[GD_EVIL >> 3]);
    SETCALLGATE(*((struct Gatedesc *)gdte_ptr), GD_KT, call_fun_wrapper, 3);
    asm volatile ("lcall %0, $0" : : "i"(GD_EVIL));
}

void
umain(int argc, char **argv)
{
    // call the evil function in ring0
	ring0_call(&evil);

	// call the evil function in ring3
	evil();
}

