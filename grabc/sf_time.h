#ifndef _SF_TIME_H_
#define _SF_TIME_H_

#include <stdint.h>

typedef struct _SfDateTime SfDateTime;

/* Valid values are the same as in SYSTEMTIME on Windows. */
struct _SfDateTime
{
  uint16_t year;
  uint16_t month;
  uint16_t day;
  uint16_t hour;
  uint16_t minute;
  uint16_t second;
  uint16_t milliseconds;
};

/* Get the number of milliseconds since the system was started. */
uint64_t
sf_time_get_tick_count (void);

void
sf_time_get_local_datetime (SfDateTime *datetime);

#endif  //  _SF_TIME_H_
