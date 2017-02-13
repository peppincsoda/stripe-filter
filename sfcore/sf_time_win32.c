#include <sfcore/sf_time.h>

#include <Windows.h>

SF_CORE_API
uint64_t
sf_time_get_tick_count (void)
{
  return GetTickCount64 ();
}

SF_CORE_API
void
sf_time_get_local_datetime (SfDateTime *datetime)
{
  SYSTEMTIME system_time;

  GetLocalTime (&system_time);

  datetime->year = system_time.wYear;
  datetime->month = system_time.wMonth;
  datetime->day = system_time.wDay;
  datetime->hour = system_time.wHour;
  datetime->minute = system_time.wMinute;
  datetime->second = system_time.wSecond;
  datetime->milliseconds = system_time.wMilliseconds;
}
