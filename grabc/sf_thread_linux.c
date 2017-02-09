#include "sf_thread.h"

#include <unistd.h>

void
sf_sleep (int milliseconds)
{
  usleep (milliseconds * 1000);
}
