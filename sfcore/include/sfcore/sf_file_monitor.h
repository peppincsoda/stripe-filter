#ifndef _SF_FILE_MONITOR_H_
#define _SF_FILE_MONITOR_H_

#include <sfcore/sf_core_header.h>
#include <sfcore/sf_bool.h>

typedef struct _SfFileMonitor SfFileMonitor;

typedef void (*SfFileMonitorCallback) (SfFileMonitor *monitor,
                                       const char *path,
                                       void *context);

SF_CORE_API
SfFileMonitor *
sf_file_monitor_new (void);

SF_CORE_API
void
sf_file_monitor_free (SfFileMonitor *monitor);

/* Add a file to watch for changes. */
SF_CORE_API
bool
sf_file_monitor_add_watch (SfFileMonitor *monitor,
                           const char *path,
                           SfFileMonitorCallback callback,
                           void *context);

/* If a file has been changed, call its corresponding callback.
   Returns true if any of the files have been changed. */
SF_CORE_API
bool
sf_file_monitor_poll (SfFileMonitor *monitor);

#endif  //  _SF_FILE_MONITOR_H_
