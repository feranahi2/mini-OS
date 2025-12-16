#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    printf(2, "Uso: strace [on|off]\n");
    printf(2, "  strace on    - Activa el trace de syscalls\n");
    printf(2, "  strace off   - Desactiva el trace de syscalls\n");
    exit();
  }

  if(strcmp(argv[1], "on") == 0) {
    trace(1);
    printf(1, "Trace de syscalls ACTIVADO\n");
  } else if(strcmp(argv[1], "off") == 0) {
    trace(0);
    printf(1, "Trace de syscalls DESACTIVADO\n");
  } else {
    printf(2, "Argumento inválido: %s\n", argv[1]);
    printf(2, "Use 'on' o 'off'\n");
    exit();
  }

  exit();
}