#include "sf_file.h"
#include "sf_logger.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <stdint.h>

/* Equation from: https://msdn.microsoft.com/en-us/library/windows/desktop/ms724228(v=vs.85).aspx */

static int64_t
to_filetime (time_t t)
{
  return (int64_t) t * 10000000 + 116444736000000000;
}

int64_t
sf_file_get_last_write_time (const char *path)
{
  struct stat file_info;

  errno = 0;
  if (stat (path, &file_info) == -1) {
    sf_log_last_sys_error ("While doing stat on file: %s", path);
    return -1;
  }

  return to_filetime (file_info.st_mtime);
}
