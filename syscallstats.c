#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"

// Nombres de syscalls para mostrar
char *syscall_names[] = {
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

int
main(int argc, char *argv[])
{
  int syscall_num;
  int count;
  int i;

  if(argc == 1) {
    // Sin parámetros: mostrar todas las syscalls
    printf(1, "\n=== ESTADISTICAS DE LLAMADAS AL SISTEMA ===\n\n");
    printf(1, "SYSCALL          INVOCACIONES\n");
    printf(1, "------------------------------------\n");
    
    // Mostrar todas las syscalls (1-23)
    for(i = 1; i <= 23; i++) {
      count = syscallstats(i);
      if(count > 0) {  // Solo mostrar syscalls que se han usado
        char *name = syscall_names[i];
        
        printf(1, "%s", name);
        
        // Agregar espacios para alinear (simple padding)
        int len = 0;
        char *p = name;
        while(*p++) len++;
        while(len < 17) {
          printf(1, " ");
          len++;
        }
        
        printf(1, "%d\n", count);
      }
    }
    
    printf(1, "------------------------------------\n");
    printf(1, "\n");
    
  } else if(argc == 2) {
    // Con parámetro: mostrar una syscall específica
    syscall_num = atoi(argv[1]);
    
    if(syscall_num < 1 || syscall_num > 23) {
      printf(2, "Error: número de syscall inválido\n");
      printf(2, "Uso: syscallstats [numero_syscall]\n");
      printf(2, "  Números válidos: 1-23\n");
      printf(2, "  Sin parámetros muestra todas las syscalls\n");
      exit();
    }
    
    count = syscallstats(syscall_num);
    printf(1, "Syscall #%d (%s): %d invocaciones\n", 
           syscall_num,
           syscall_names[syscall_num],
           count);
  } else {
    printf(2, "Uso: syscallstats [numero_syscall]\n");
    printf(2, "  Sin parámetros: muestra todas las syscalls\n");
    printf(2, "  Con número: muestra solo esa syscall\n");
    exit();
  }

  exit();
}
