#ifndef _SF_FILE_H_
#define _SF_FILE_H_

#include <stdint.h>

/* Returns the values from the FILETIME structure as on Windows. */
int64_t
sf_file_get_last_write_time (const char *path);

#endif  //  _SF_FILE_H_
