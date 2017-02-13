#include <sfcore/sf_time.h>

#include <time.h>
#include <assert.h>

uint64_t
sf_time_get_tick_count (void)
{
  struct timespec now;
  int ret;
  
  ret = clock_gettime (CLOCK_MONOTONIC, &now);
  assert (ret == 0); // It is assumed that the system has a monotonic clock
  
  return (uint64_t) now.tv_sec * 1000.0 + (uint64_t) now.tv_nsec / 1000000.0;
}

void
sf_time_get_local_datetime (SfDateTime *datetime)
{
  struct timespec now;
  struct tm tdata;
  
  clock_gettime (CLOCK_REALTIME, &now);

  localtime_r (&now.tv_sec, &tdata);

  datetime->year = tdata.tm_year + 1900;
  datetime->month = tdata.tm_mon + 1;
  datetime->day = tdata.tm_mday;
  datetime->hour = tdata.tm_hour;
  datetime->minute = tdata.tm_min;
  datetime->second = tdata.tm_sec;
  datetime->milliseconds = now.tv_nsec / 1000000;
}
