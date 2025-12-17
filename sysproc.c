#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Declaración externa de la variable global
extern int syscall_trace;

// Nueva syscall para activar/desactivar el trace
int
sys_trace(void)
{
  int enable;
  
  if(argint(0, &enable) < 0)
    return -1;
    
  syscall_trace = enable ? 1 : 0;
  return 0;
}

int sys_getactiveprocs(void)
{
   return count_active_procs();
}

// Declaraciones externas para syscallstats
extern int get_syscall_count(int);
extern char* get_syscall_name(int);

// Nueva syscall para obtener estadísticas de syscalls
// Si recibe -1, devuelve el total de syscalls definidas (22)
// Si recibe un número válido (1-22), devuelve el contador de esa syscall
int
sys_syscallstats(void)
{
  int syscall_num;
  
  if(argint(0, &syscall_num) < 0)
    return -1;
  
  // Si es -1, retornar el número total de syscalls
  if(syscall_num == -1)
    return 22;  // Total de syscalls sin contar trace y syscallstats
  
  // Retornar el contador de la syscall específica
  return get_syscall_count(syscall_num);
}
