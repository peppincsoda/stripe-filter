#ifndef _SF_FILE_H_
#define _SF_FILE_H_

#include <sfcore/sf_core_header.h>

#include <stdint.h>

/* Returns the values from the FILETIME structure as on Windows. */
SF_CORE_API
int64_t
sf_file_get_last_write_time (const char *path);

#endif  //  _SF_FILE_H_
