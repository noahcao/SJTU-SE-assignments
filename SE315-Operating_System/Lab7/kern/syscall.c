/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/syscall.h>
#include <kern/console.h>
#include <kern/sched.h>
#include <kern/time.h>
#include <kern/e1000.h>
#include <kern/spinlock.h>

// Print a string to the system console.
// The string is exactly 'len' characters long.
// Destroys the environment on memory errors.
static void
sys_cputs(const char *s, size_t len)
{
	// Check that the user has permission to read memory [s, s+len).
	// Destroy the environment if not.

	// LAB 3: Your code here.
	user_mem_assert(curenv, (const void *)s, len, PTE_U);
	// Print the string supplied by the user.
	cprintf("%.*s", len, s);
}

// Read a character from the system console without blocking.
// Returns the character, or 0 if there is no input waiting.
static int
sys_cgetc(void)
{
	return cons_getc();
}

// Returns the current environment's envid.
static envid_t
sys_getenvid(void)
{
	return curenv->env_id;
}

// Destroy a given environment (possibly the currently running environment).
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_destroy(envid_t envid)
{
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 1)) < 0)
		return r;
	env_destroy(e);
	return 0;
}

// Deschedule current environment and pick a different one to run.
static void
sys_yield(void)
{
	sched_yield();
}

// Allocate a new environment.
// Returns envid of new environment, or < 0 on error.  Errors are:
//	-E_NO_FREE_ENV if no free environment is available.
//	-E_NO_MEM on memory exhaustion.
static envid_t
sys_exofork(void)
{
	// Create the new environment with env_alloc(), from kern/env.c.
	// It should be left as env_alloc created it, except that
	// status is set to ENV_NOT_RUNNABLE, and the register set is copied
	// from the current environment -- but tweaked so sys_exofork
	// will appear to return 0.

	// LAB 4: Your code here.
	int r;
	struct Env *e;

	r = env_alloc_fork(&e, curenv->env_id);
	if (r < 0)
	{
		return r;
	}

	e->env_status = ENV_NOT_RUNNABLE;
	e->env_tf = curenv->env_tf;
	e->env_tf.tf_regs.reg_eax = 0;

	return e->env_id;
}

// Set envid's env_status to status, which must be ENV_RUNNABLE
// or ENV_NOT_RUNNABLE.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if status is not a valid status for an environment.
static int
sys_env_set_status(envid_t envid, int status)
{
	// Hint: Use the 'envid2env' function from kern/env.c to translate an
	// envid to a struct Env.
	// You should set envid2env's third argument to 1, which will
	// check whether the current environment has permission to set
	// envid's status.

	// LAB 4: Your code here.
	int r;
	struct Env *e;
	r = envid2env(envid, &e, 1);
	if (r < 0)
	{
		return -E_BAD_ENV;
	}

	assert(e);
	if (e->env_status == ENV_RUNNABLE || e->env_status == ENV_NOT_RUNNABLE)
	{
		e->env_status = status;
		return 0;
	}
	return -E_INVAL;
}

// Set envid's trap frame to 'tf'.
// tf is modified to make sure that user environments always run at code
// protection level 3 (CPL 3), interrupts enabled, and IOPL of 0.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_set_trapframe(envid_t envid, struct Trapframe *tf)
{
	// LAB 5: Your code here.
	// Remember to check whether the user has supplied us with a good
	// address!
	int r;
	struct Env *e;
	r = envid2env(envid, &e, 1);
	if (r < 0)
	{
		return -E_BAD_ENV;
	}

	user_mem_assert(curenv, tf, sizeof(struct Trapframe), 0);
	e->env_tf = *tf;
	e->env_tf.tf_cs |= 3;
	e->env_tf.tf_eflags |= FL_IF;
	e->env_tf.tf_eflags &= (~FL_IOPL_MASK);

	return 0;
}

// Set the page fault upcall for 'envid' by modifying the corresponding struct
// Env's 'env_pgfault_upcall' field.  When 'envid' causes a page fault, the
// kernel will push a fault record onto the exception stack, then branch to
// 'func'.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_set_pgfault_upcall(envid_t envid, void *func)
{
	// LAB 4: Your code here.
	int r;
	struct Env *e;

	r = envid2env(envid, &e, 1);
	if (r < 0)
	{
		return -E_BAD_ENV;
	}

	e->env_pgfault_upcall = func;
	return 0;
}

// Allocate a page of memory and map it at 'va' with permission
// 'perm' in the address space of 'envid'.
// The page's contents are set to 0.
// If a page is already mapped at 'va', that page is unmapped as a
// side effect.
//
// perm -- PTE_U | PTE_P must be set, PTE_AVAIL | PTE_W may or may not be set,
//         but no other bits may be set.  See PTE_SYSCALL in inc/mmu.h.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
//	-E_INVAL if perm is inappropriate (see above).
//	-E_NO_MEM if there's no memory to allocate the new page,
//		or to allocate any necessary page tables.
static int
sys_page_alloc(envid_t envid, void *va, int perm)
{
	// Hint: This function is a wrapper around page_alloc() and
	//   page_insert() from kern/pmap.c.
	//   Most of the new code you write should be to check the
	//   parameters for correctness.
	//   If page_insert() fails, remember to free the page you
	//   allocated!

	// LAB 4: Your code here.
	int r;
	struct Env *e;

	r = envid2env(envid, &e, 1);
	if (r < 0)
	{
		return -E_BAD_ENV;
	}

	if ((uintptr_t)va >= UTOP || ((uintptr_t)va % PGSIZE) != 0)
	{
		return -E_INVAL;
	}

	if ((perm & (PTE_U | PTE_P)) == (PTE_U | PTE_P))
	{
		if ((perm & ~PTE_SYSCALL) > 0)
		{
			return -E_INVAL;
		}

		struct PageInfo *p = page_alloc(ALLOC_ZERO);
		if (p == NULL)
		{
			return -E_NO_MEM;
		}

		r = page_insert(e->env_pgdir, p, va, perm);
		if (r < 0)
		{
			page_free(p);
			return r;
		}
		memmove(e->env_kern_pgdir + PDX(va), e->env_pgdir + PDX(va), sizeof(pde_t));

		return 0;
	}

	return -E_INVAL;
}

// Map the page of memory at 'srcva' in srcenvid's address space
// at 'dstva' in dstenvid's address space with permission 'perm'.
// Perm has the same restrictions as in sys_page_alloc, except
// that it also must not grant write access to a read-only
// page.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if srcenvid and/or dstenvid doesn't currently exist,
//		or the caller doesn't have permission to change one of them.
//	-E_INVAL if srcva >= UTOP or srcva is not page-aligned,
//		or dstva >= UTOP or dstva is not page-aligned.
//	-E_INVAL is srcva is not mapped in srcenvid's address space.
//	-E_INVAL if perm is inappropriate (see sys_page_alloc).
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in srcenvid's
//		address space.
//	-E_NO_MEM if there's no memory to allocate any necessary page tables.
static int
sys_page_map(envid_t srcenvid, void *srcva,
						 envid_t dstenvid, void *dstva, int perm)
{
	// Hint: This function is a wrapper around page_lookup() and
	//   page_insert() from kern/pmap.c.
	//   Again, most of the new code you write should be to check the
	//   parameters for correctness.
	//   Use the third argument to page_lookup() to
	//   check the current permissions on the page.

	// LAB 4: Your code here.
	int r;
	struct Env *src_env;
	struct Env *dst_env;

	if ((r = envid2env(srcenvid, &src_env, 1)) < 0)
	{
		return r;
	}
	if ((r = envid2env(dstenvid, &dst_env, 1)) < 0)
	{
		return r;
	}

	if ((uintptr_t)srcva >= UTOP || (uintptr_t)srcva % PGSIZE ||
			(uintptr_t)dstva >= UTOP || (uintptr_t)dstva % PGSIZE)
	{
		return -E_INVAL;
	}

	if ((perm & (PTE_U | PTE_P)) != (PTE_U | PTE_P) || perm & ~PTE_SYSCALL)
	{
		return -E_INVAL;
	}

	pte_t *pte;
	struct PageInfo *p = page_lookup(src_env->env_pgdir, srcva, &pte);
	if (p == NULL)
	{
		return -E_INVAL;
	}

	if ((perm & PTE_W) && !(*pte & PTE_W))
	{
		return -E_INVAL;
	}
	if ((r = page_insert(dst_env->env_pgdir, p, dstva, perm)) < 0)
	{
		return r;
	}
	memmove(dst_env->env_kern_pgdir + PDX(dstva), dst_env->env_pgdir + PDX(dstva), sizeof(pde_t));

	return r;
}

// Unmap the page of memory at 'va' in the address space of 'envid'.
// If no page is mapped, the function silently succeeds.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
static int
sys_page_unmap(envid_t envid, void *va)
{
	// Hint: This function is a wrapper around page_remove().

	// LAB 4: Your code here.
	int r;
	struct Env *e;

	r = envid2env(envid, &e, 1);
	if (r < 0)
	{
		return r;
	}

	if ((uintptr_t)va >= UTOP || (uintptr_t)va % PGSIZE)
	{
		return -E_INVAL;
	}

	page_remove(e->env_pgdir, va);
	memmove(e->env_kern_pgdir + PDX(va), e->env_pgdir + PDX(va), sizeof(pde_t));
	return 0;
}

// Try to send 'value' to the target env 'envid'.
// If srcva < UTOP, then also send page currently mapped at 'srcva',
// so that receiver gets a duplicate mapping of the same page.
//
// The send fails with a return value of -E_IPC_NOT_RECV if the
// target is not blocked, waiting for an IPC.
//
// The send also can fail for the other reasons listed below.
//
// Otherwise, the send succeeds, and the target's ipc fields are
// updated as follows:
//    env_ipc_recving is set to 0 to block future sends;
//    env_ipc_from is set to the sending envid;
//    env_ipc_value is set to the 'value' parameter;
//    env_ipc_perm is set to 'perm' if a page was transferred, 0 otherwise.
// The target environment is marked runnable again, returning 0
// from the paused sys_ipc_recv system call.  (Hint: does the
// sys_ipc_recv function ever actually return?)
//
// If the sender wants to send a page but the receiver isn't asking for one,
// then no page mapping is transferred, but no error occurs.
// The ipc only happens when no errors occur.
//
// Returns 0 on success, < 0 on error.
// Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist.
//		(No need to check permissions.)
//	-E_IPC_NOT_RECV if envid is not currently blocked in sys_ipc_recv,
//		or another environment managed to send first.
//	-E_INVAL if srcva < UTOP but srcva is not page-aligned.
//	-E_INVAL if srcva < UTOP and perm is inappropriate
//		(see sys_page_alloc).
//	-E_INVAL if srcva < UTOP but srcva is not mapped in the caller's
//		address space.
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in the
//		current environment's address space.
//	-E_NO_MEM if there's not enough memory to map srcva in envid's
//		address space.
static int
sys_ipc_try_send(envid_t envid, uint32_t value, void *srcva, unsigned perm)
{
	// LAB 4: Your code here.
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 0) < 0))
	{
		return -E_BAD_ENV;
	}
	// if (!e->env_ipc_recving)
	// {
	// 	return -E_IPC_NOT_RECV;
	// }

	if ((uintptr_t)srcva < UTOP && (((uintptr_t)e->env_ipc_dstva < UTOP && e->env_ipc_recving) || !e->env_ipc_recving))
	{
		if ((uintptr_t)srcva % PGSIZE ||
				(perm & (PTE_U | PTE_P)) != (PTE_U | PTE_P) ||
				(perm & (~PTE_SYSCALL)))
		{
			return -E_INVAL;
		}

		pte_t *pte;
		struct PageInfo *p = page_lookup(curenv->env_pgdir, srcva, &pte);
		if (p == NULL)
		{
			return -E_INVAL;
		}

		if ((perm & PTE_W) && ((*pte & PTE_W) == 0))
		{
			return -E_INVAL;
		}

		if (e->env_ipc_recving && (uintptr_t)e->env_ipc_dstva < UTOP)
		{
			if ((r = page_insert(e->env_pgdir, p, e->env_ipc_dstva, perm)) < 0)
			{
				return r;
			}
			memmove(e->env_kern_pgdir + PDX(e->env_ipc_dstva), e->env_pgdir + PDX(e->env_ipc_dstva), sizeof(pde_t));
		}
		else
		{
			curenv->env_ipc_perm_pending = perm;
			curenv->env_ipc_page_pending = p;
		}
	}

	if (e->env_ipc_recving)
	{
		e->env_ipc_recving = 0;
		e->env_ipc_from = curenv->env_id;
		e->env_ipc_value = value;
		e->env_ipc_perm = perm;
		e->env_status = ENV_RUNNABLE;
		e->env_tf.tf_regs.reg_eax = 0;
	}
	else
	{
		curenv->env_ipc_to_pending = envid;
		curenv->env_ipc_value_pending = value;
		curenv->env_status = ENV_NOT_RUNNABLE;
		sched_yield();
	}

	return 0;
}

// Block until a value is ready.  Record that you want to receive
// using the env_ipc_recving and env_ipc_dstva fields of struct Env,
// mark yourself not runnable, and then give up the CPU.
//
// If 'dstva' is < UTOP, then you are willing to receive a page of data.
// 'dstva' is the virtual address at which the sent page should be mapped.
//
// This function only returns on error, but the system call will eventually
// return 0 on success.
// Return < 0 on error.  Errors are:
//	-E_INVAL if dstva < UTOP but dstva is not page-aligned.

static int
sys_ipc_recv(void *dstva)
{
	// LAB 4: Your code here.
	// if (dstva >= (void *)UTOP || !PGOFF(dstva))
	// {
	// curenv->env_ipc_recving = 1;
	// curenv->env_ipc_dstva = dstva;
	// curenv->env_status = ENV_NOT_RUNNABLE;
	// sched_yield();
	// return 0;
	// }
	// return -E_INVAL;
	int r;
	struct Env *e;
	if (dstva < (void *)UTOP && (uintptr_t)dstva % PGSIZE)
	{
		return -E_INVAL;
	}

	for (int i = 0; i < NENV; i++)
	{
		e = &envs[i];
		if (e->env_status == ENV_NOT_RUNNABLE && e->env_ipc_to_pending == curenv->env_id)
		{
			if (dstva < (void *)UTOP && e->env_ipc_page_pending != NULL)
			{
				if ((r = page_insert(curenv->env_pgdir, e->env_ipc_page_pending, dstva, e->env_ipc_perm_pending)) < 0)
				{
					return r;
				}
				memmove(curenv->env_kern_pgdir + PDX(dstva), curenv->env_pgdir + PDX(dstva), sizeof(pde_t));
				curenv->env_ipc_perm = e->env_ipc_perm_pending;
			}
			curenv->env_ipc_from = e->env_id;
			curenv->env_ipc_value = e->env_ipc_value_pending;
			e->env_ipc_to_pending = 0;
			e->env_status = ENV_RUNNABLE;
			e->env_tf.tf_regs.reg_eax = 0;
			return 0;
		}
	}

	curenv->env_ipc_recving = 1;
	curenv->env_ipc_dstva = dstva;
	curenv->env_status = ENV_NOT_RUNNABLE;
	sched_yield();
	return 0;
}

static int
sys_map_kernel_page(void *kpage, void *va)
{
	int r;
	struct PageInfo *p = pa2page(PADDR(kpage));
	if (p == NULL)
		return -E_INVAL;

	if ((r = page_insert(curenv->env_pgdir, p, va, PTE_U | PTE_W)) < 0)
		return r;

	memmove(curenv->env_kern_pgdir + PDX(va), curenv->env_pgdir + PDX(va), sizeof(pde_t));

	return r;
}

static int
sys_sbrk(uint32_t inc)
{
	// LAB3: your code here.
	// Align inc to whole pages.
	uint32_t inc_size = ROUNDUP(inc, PGSIZE);

	// Prevent heap address range from overflowing to kernel.
	if (curenv->env_heap + inc_size > ULIM || curenv->env_heap + inc_size < curenv->env_heap){
		cprintf("[%08x] sbrk out of range", curenv->env_id);
		env_destroy(curenv);
		return -1;
	}

	// Allocate more space, increase brk pointer.
	region_alloc(curenv, (void *)curenv->env_heap, inc_size);
	curenv->env_heap += inc_size;
	return curenv->env_heap;
}

// Return the current time.
static int
sys_time_msec(void){
	// LAB 6: Your code here.
	return time_msec();
}

int sys_net_send(const void *buf, uint32_t len)
{
	// LAB 6: Your code here.
	// Check the user permission to [buf, buf + len]
	// Call e1000_tx to send the packet
	// Hint: e1000_tx only accept kernel virtual address
	user_mem_assert(curenv, buf, len, 0);
	return e1000_tx(buf, len);
}

int sys_net_recv(void *buf, uint32_t len){
	// LAB 6: Your code here.
	// Check the user permission to [buf, buf + len]
	// Call e1000_rx to fill the buffer
	// Hint: e1000_rx only accept kernel virtual address
	user_mem_assert(curenv, buf, len, 0);
	return e1000_rx(buf, len);
}

static int
sys_read_mac(uint8_t *mac_addr){
	user_mem_assert(curenv, mac_addr, 6, PTE_W);
	memmove(mac_addr, e1000_mac_address, 6);
	return 0;
}

int32_t
_syscall(uint32_t syscallno, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, struct Trapframe *tf)
{
	int ret;

	lock_kernel();
	curenv->env_tf = *tf;
	ret = syscall(syscallno, a1, a2, a3, a4, a5);
	unlock_kernel();

	return ret;
}
// Dispatches to the correct kernel function, passing the arguments.
int32_t
syscall(uint32_t syscallno, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
	// Call the function corresponding to the 'syscallno' parameter.
	// Return any appropriate return value.
	// LAB 3: Your code here.
	int r = 0;
    envid_t envid = (envid_t)a1;
    void* arg_a2 = (void*)a2;
	switch (syscallno) {
		case SYS_cputs: 
			sys_cputs((char*)a1, a2);
			return 0;
		case SYS_cgetc:
			return sys_cgetc();
		case SYS_getenvid:
			return sys_getenvid();
		case SYS_env_destroy:
			sys_env_destroy(a1);
			return 0;
		case SYS_map_kernel_page:
			sys_map_kernel_page((void*)a1, arg_a2);
			return 0;
		case SYS_sbrk:
			return sys_sbrk((uint32_t)a1);
		case SYS_yield:
			sys_yield();
			return 0;
		case SYS_exofork:
		 	return sys_exofork();
		case SYS_env_set_status:
			return sys_env_set_status(envid, (int)a2);
		case SYS_page_alloc:
			return sys_page_alloc(envid, arg_a2, (int)a3);
		case SYS_page_map:
			return sys_page_map(envid, arg_a2, (envid_t)a3, (void *)a4, (int)a5);
		case SYS_page_unmap:
		 	return sys_page_unmap(envid, arg_a2);
		case SYS_env_set_pgfault_upcall:
      		return sys_env_set_pgfault_upcall(envid, arg_a2);
		case SYS_ipc_try_send:
			return sys_ipc_try_send(envid, (uint32_t)a2, (void *)a3, (unsigned)a4);
        case SYS_net_send:
		    return sys_net_send((const void *)a1, (uint32_t)(a2));
        case SYS_net_recv:
            return sys_net_recv((void*)a1, a2);
		case SYS_ipc_recv:
  			return sys_ipc_recv((void *)a1);
        case SYS_time_msec:
            return sys_time_msec();
        case SYS_env_set_trapframe:
            return sys_env_set_trapframe((envid_t)a1, (struct Trapframe*)a2);
        case SYS_read_mac:
            return sys_read_mac((uint8_t *)a1);
		default:
			return -E_INVAL;
	}
}


