#include "sf_logger.h"
#include "sf_time.h"

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

typedef enum _SfLoggerLevel SfLoggerLevel;

enum _SfLoggerLevel
{
  SfLoggerLevel_Info,
  SfLoggerLevel_Warning,
  SfLoggerLevel_Error,
};

static void
sf_log_write (SfLoggerLevel level, const char *format, va_list vl)
{
  const char *level_str = NULL;
  char buffer[256];
  SfDateTime datetime;

  switch (level) {
  case SfLoggerLevel_Info: level_str = "INFO"; break;
  case SfLoggerLevel_Warning: level_str = "WARNING"; break;
  case SfLoggerLevel_Error: level_str = "ERROR"; break;
  default: assert (0);
  }

#if defined (_MSC_VER) && defined (_WIN32)
  vsnprintf_s (buffer, sizeof (buffer), _TRUNCATE, format, vl);
#else
  vsnprintf (buffer, sizeof (buffer), format, vl);
#endif

  sf_time_get_local_datetime (&datetime);
  
  fprintf (stdout,
    "%04d-%02d-%02d %02d:%02d:%02d:%03d %s:%s\n",
    datetime.year,
    datetime.month,
    datetime.day,
    datetime.hour,
    datetime.minute,
    datetime.second,
    datetime.milliseconds,
    level_str,
    buffer);
  fflush (stdout);
}

void
sf_log_info (const char *format, ...)
{
  va_list vl;
  va_start (vl, format);
  sf_log_write (SfLoggerLevel_Info, format, vl);
  va_end (vl);
}

void
sf_log_warning (const char *format, ...)
{
  va_list vl;
  va_start (vl, format);
  sf_log_write (SfLoggerLevel_Warning, format, vl);
  va_end (vl);
}

void
sf_log_error (const char *format, ...)
{
  va_list vl;
  va_start (vl, format);
  sf_log_write (SfLoggerLevel_Error, format, vl);
  va_end (vl);
}
