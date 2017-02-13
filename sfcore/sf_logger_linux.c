#include "sf_logger.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>

void
sf_log_last_sys_error (const char *format, ...)
{
  va_list vl;
  char userMsgBuf[256];
  int errnum = errno;
  char msgBuf[256];

  va_start (vl, format);
  vsnprintf (userMsgBuf, sizeof (userMsgBuf), format, vl);

  strerror_r (errnum, msgBuf, sizeof (msgBuf));

  sf_log_error ("%s; %s", userMsgBuf, msgBuf);

  va_end (vl);
}
