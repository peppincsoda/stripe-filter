#include "sf_file.h"
#include "sf_logger.h"

#include <Windows.h>

int64_t
sf_file_get_last_write_time (const char *path)
{
  HANDLE hFile;
  FILETIME lastWriteTime;
  int64_t ret = -1;

  hFile = CreateFile(path,
    GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    0,
    NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    sf_log_last_sys_error ("While opening file: %s", path);
    goto cleanup;
  }
  
  if (!GetFileTime (hFile, NULL, NULL, &lastWriteTime)) {
    sf_log_last_sys_error ("While getting file time: %s", path);
    goto cleanup;
  }

  ret = (((int64_t) lastWriteTime.dwHighDateTime) << 32) |
          (int64_t) lastWriteTime.dwLowDateTime;

cleanup:
  CloseHandle (hFile);
  return ret;
}
