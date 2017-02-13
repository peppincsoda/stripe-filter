#include <sfcore/sf_thread.h>

#include <Windows.h>

SF_CORE_API
void
sf_sleep (int milliseconds)
{
  Sleep (milliseconds);
}
