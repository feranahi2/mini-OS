#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int ticks = uptime();
  int seconds = ticks / 100;

  printf(1, "Uptime: %d ticks (%d seconds)\n", ticks, seconds);

  int active = getactiveprocs();
  printf(1, "Active processes: %d\n", active);

  exit();
}

