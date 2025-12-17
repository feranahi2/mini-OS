#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"

// Variable global para controlar el syscall trace
int syscall_trace = 0;

// Array de contadores para cada syscall
static int syscall_counts[24];

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  struct proc *curproc = myproc();

  if(addr >= curproc->sz || addr+4 > curproc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;
  struct proc *curproc = myproc();

  if(addr >= curproc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)curproc->sz;
  for(s = *pp; s < ep; s++){
    if(*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  struct proc *curproc = myproc();
 
  if(argint(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= curproc->sz || (uint)i+size > curproc->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_trace(void);
extern int sys_syscallstats(void);


static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_trace]   sys_trace,
[SYS_syscallstats] sys_syscallstats,

};

static char *syscall_names[] = {
[SYS_fork]    "fork",
[SYS_exit]    "exit", 
[SYS_wait]    "wait",
[SYS_pipe]    "pipe",
[SYS_read]    "read",
[SYS_kill]    "kill",
[SYS_exec]    "exec",
[SYS_fstat]   "fstat",
[SYS_chdir]   "chdir",
[SYS_dup]     "dup",
[SYS_getpid]  "getpid",
[SYS_sbrk]    "sbrk",
[SYS_sleep]   "sleep",
[SYS_uptime]  "uptime",
[SYS_open]    "open",
[SYS_write]   "write",
[SYS_mknod]   "mknod",
[SYS_unlink]  "unlink",
[SYS_link]    "link",
[SYS_mkdir]   "mkdir",
[SYS_close]   "close",
[SYS_trace]   "trace",
[SYS_syscallstats] "syscallstats",
};


// Función para obtener el contador de una syscall específica
int
get_syscall_count(int syscall_num)
{
  if(syscall_num >= 0 && syscall_num < 24)
    return syscall_counts[syscall_num];
  return -1;
}

// Función para obtener el nombre de una syscall
char*
get_syscall_name(int syscall_num)
{
  if(syscall_num >= 0 && syscall_num < NELEM(syscall_names) && syscall_names[syscall_num])
    return syscall_names[syscall_num];
  return "unknown";
}


// Función para mostrar argumentos de syscall
void
print_syscall_args(int num)
{
  int arg0, arg1, arg2;
  char *str;
  
  switch(num) {
    case SYS_fork:
    case SYS_exit:
    case SYS_wait:
    case SYS_getpid:
    case SYS_uptime:
      cprintf("()");
      break;
    
    case SYS_kill:
    case SYS_sleep:
    case SYS_close:
    case SYS_dup:
      if(argint(0, &arg0) >= 0)
        cprintf("(%d)", arg0);
      else
        cprintf("(?)");
      break;
    
    case SYS_sbrk:
      if(argint(0, &arg0) >= 0)
        cprintf("(%d)", arg0);
      else
        cprintf("(?)");
      break;
    
    case SYS_read:
    case SYS_write:
      if(argint(0, &arg0) >= 0 && argint(2, &arg2) >= 0)
        cprintf("(%d, buf, %d)", arg0, arg2);
      else
        cprintf("(?, ?, ?)");
      break;
    
    case SYS_open:
      if(argstr(0, &str) >= 0 && argint(1, &arg1) >= 0)
        cprintf("(\"%s\", %d)", str, arg1);
      else
        cprintf("(?, ?)");
      break;
    
    case SYS_chdir:
    case SYS_unlink:
    case SYS_mkdir:
      if(argstr(0, &str) >= 0)
        cprintf("(\"%s\")", str);
      else
        cprintf("(?)");
      break;
    
    case SYS_mknod:
      if(argstr(0, &str) >= 0 && argint(1, &arg1) >= 0 && argint(2, &arg2) >= 0)
        cprintf("(\"%s\", %d, %d)", str, arg1, arg2);
      else
        cprintf("(?, ?, ?)");
      break;
    
    case SYS_link:
      if(argstr(0, &str) >= 0) {
        char *str2;
        if(argstr(1, &str2) >= 0)
          cprintf("(\"%s\", \"%s\")", str, str2);
        else
          cprintf("(\"%s\", ?)", str);
      } else {
        cprintf("(?, ?)");
      }
      break;
    
    case SYS_exec:
      if(argstr(0, &str) >= 0)
        cprintf("(\"%s\", argv)", str);
      else
        cprintf("(?, ?)");
      break;
    
    case SYS_pipe:
      cprintf("(pipefd)");
      break;
    
    case SYS_fstat:
      if(argint(0, &arg0) >= 0)
        cprintf("(%d, stat)", arg0);
      else
        cprintf("(?, ?)");
      break;
      
    case SYS_trace:
      if(argint(0, &arg0) >= 0)
        cprintf("(%d)", arg0);
      else
        cprintf("(?)");
      break;
    
    default:
      cprintf("(...)");
      break;
  }
}

void
syscall(void)
{
  int num;
  struct proc *curproc = myproc();

  num = curproc->tf->eax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    // Incrementar el contador de esta syscall
    if(num < 24)
      syscall_counts[num]++;

    // Si esta activo el trace se mostrara el syscall
    if(syscall_trace) {
      cprintf("[PID %d] %s: %s", curproc->pid, curproc->name, 
              num < NELEM(syscall_names) ? syscall_names[num] : "unknown");
      print_syscall_args(num);
      cprintf("\n");
    }

    curproc->tf->eax = syscalls[num]();
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}
