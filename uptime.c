#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
   int ticks = uptime();
   printf(1, "Uptime:%d ticks(%d segundos)\n",ticks, ticks/100);
   exit();
}
