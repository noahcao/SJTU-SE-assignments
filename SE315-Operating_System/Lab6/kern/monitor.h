#ifndef JOS_KERN_MONITOR_H
#define JOS_KERN_MONITOR_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

struct Trapframe;

// Activate the kernel monitor,
// optionally providing a trap frame indicating the current state
// (NULL if none).
void monitor(struct Trapframe *tf);

// Functions implementing monitor commands.
int mon_help(int argc, char **argv, struct Trapframe *tf);
int mon_kerninfo(int argc, char **argv, struct Trapframe *tf);
int mon_backtrace(int argc, char **argv, struct Trapframe *tf);
int mon_time(int argc, char **argv, struct Trapframe *tf);
int showmappings(int argc, char **argv, struct Trapframe *tf);
int setperm(int argc, char **argv, struct Trapframe *tf);
int clearperm(int argc, char **argv, struct Trapframe *tf);
int dumppa(int argc, char **argv, struct Trapframe *tf);
int dumpva(int argc, char **argv, struct Trapframe *tf);

#endif	// !JOS_KERN_MONITOR_H
