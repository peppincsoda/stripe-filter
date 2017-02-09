#include "sf_thread.h"

#include <Windows.h>

void
sf_sleep (int milliseconds)
{
  Sleep (milliseconds);
}
