#include <sfcore/sf_memory.h>
#include <sfcore/sf_logger.h>

#include <Windows.h>

#include <stdio.h>
#include <stdarg.h>

SF_CORE_API
void
sf_log_last_sys_error (const char *format, ...)
{
  va_list vl;
  char userMsgBuf[256];
  DWORD dw = GetLastError ();
  CHAR msgBuf[256];

  va_start (vl, format);
  _vsprintf_p (userMsgBuf, ARRAY_SIZE (userMsgBuf), format, vl);

  FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    dw,
    MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
    msgBuf,
    ARRAY_SIZE (msgBuf),
    NULL);

  sf_log_error ("%s; %s", userMsgBuf, msgBuf);

  va_end (vl);
}
