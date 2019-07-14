/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.section .user_mapped.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */

TRAPHANDLER_NOEC(divide_error, T_DIVIDE)
TRAPHANDLER_NOEC(debug_exception, T_DEBUG)
TRAPHANDLER_NOEC(non_maskable_interrupt, T_NMI)
TRAPHANDLER_NOEC(break_point, T_BRKPT)
TRAPHANDLER_NOEC(overflow, T_OFLOW)
TRAPHANDLER_NOEC(bounds_check, T_BOUND)
TRAPHANDLER_NOEC(illegal_opcode, T_ILLOP)
TRAPHANDLER_NOEC(device_not_available, T_DEVICE)
TRAPHANDLER(double_fault, T_DBLFLT)
TRAPHANDLER(invalid_task_switch_segment, T_TSS)
TRAPHANDLER(segment_not_present, T_SEGNP)
TRAPHANDLER(stack_fault, T_STACK)
TRAPHANDLER(general_protection_fault, T_GPFLT)
TRAPHANDLER(page_fault, T_PGFLT)
TRAPHANDLER_NOEC(floating_point_error, T_FPERR)
TRAPHANDLER(alignment_check, T_ALIGN)
TRAPHANDLER_NOEC(machine_check, T_MCHK)
TRAPHANDLER_NOEC(SIMD_floating_point_exception, T_SIMDERR)
TRAPHANDLER_NOEC(system_call, T_SYSCALL)

TRAPHANDLER_NOEC( irq_entry_0  , IRQ_OFFSET+ 0 ) 
TRAPHANDLER_NOEC( irq_entry_1  , IRQ_OFFSET+ 1 ) 
TRAPHANDLER_NOEC( irq_entry_2  , IRQ_OFFSET+ 2 ) 
TRAPHANDLER_NOEC( irq_entry_3  , IRQ_OFFSET+ 3 ) 
TRAPHANDLER_NOEC( irq_entry_4  , IRQ_OFFSET+ 4 ) 
TRAPHANDLER_NOEC( irq_entry_5  , IRQ_OFFSET+ 5 ) 
TRAPHANDLER_NOEC( irq_entry_6  , IRQ_OFFSET+ 6 ) 
TRAPHANDLER_NOEC( irq_entry_7  , IRQ_OFFSET+ 7 ) 
TRAPHANDLER_NOEC( irq_entry_8  , IRQ_OFFSET+ 8 ) 
TRAPHANDLER_NOEC( irq_entry_9  , IRQ_OFFSET+ 9 ) 
TRAPHANDLER_NOEC( irq_entry_10 , IRQ_OFFSET+ 10) 
TRAPHANDLER_NOEC( irq_entry_11 , IRQ_OFFSET+ 11) 
TRAPHANDLER_NOEC( irq_entry_12 , IRQ_OFFSET+ 12) 
TRAPHANDLER_NOEC( irq_entry_13 , IRQ_OFFSET+ 13) 
TRAPHANDLER_NOEC( irq_entry_14 , IRQ_OFFSET+ 14) 
TRAPHANDLER_NOEC( irq_entry_15 , IRQ_OFFSET+ 15)

.global sysenter_handler
.type sysenter_handler,@function
.align 2
sysenter_handler:
	/* parameters for syscall*/
	pushl %edi
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %eax	
	call syscall	
	/* restore stack pointer and return address*/
	movl %ebp, %ecx
	movl %esi, %edx
	sysexit
	
	
/*
 * Lab 3: Your code here for _alltraps
 */
.globl _alltraps
.type _alltraps, @function					
.align 2
_alltraps:
	pushl %ds
    pushl %es
    pushal

    movl $GD_KD, %eax
    movw %ax, %ds
    movw %ax, %es

    pushl %esp
    call switch_and_trap
	addl $4,%esp

	popal
	popl %es
	popl %ds
	addl $0x8,%esp
	iret