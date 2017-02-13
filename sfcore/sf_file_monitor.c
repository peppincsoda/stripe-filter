#include <sfcore/sf_memory.h>
#include <sfcore/sf_file_monitor.h>
#include <sfcore/sf_linked_list.h>
#include <sfcore/sf_logger.h>
#include <sfcore/sf_time.h>
#include <sfcore/sf_file.h>

typedef struct _SfWatchedFile SfWatchedFile;

struct _SfWatchedFile
{
  SfLinkedListItem list_item;

  char *path;
  SfFileMonitorCallback callback;
  void *context;

  int64_t last_write_time;
};

struct _SfFileMonitor
{
  SfLinkedListItem watched_files;

  uint64_t last_check_time;
};

static SfWatchedFile *
sf_watched_file_new (const char *path,
                     SfFileMonitorCallback callback,
                     void *context)
{
  SfWatchedFile *p = (SfWatchedFile *) sf_malloc (sizeof (SfWatchedFile));

  p->path = sf_strcpy (path);
  p->callback = callback;
  p->context = context;

  p->last_write_time = 0;

  return p;
}

static void
sf_watched_file_free (SfWatchedFile *p)
{
  sf_clear_pointer (&p->path, sf_free);
  sf_free (p);
}

static bool
sf_watched_file_check_changes (SfWatchedFile *p)
{
  int64_t last_write_time;

  /* If checking the modification time fails once,
     we never do it again to prevent flooding the log with error messages. */
  if (p->last_write_time == -1)
    return false;

  last_write_time = sf_file_get_last_write_time (p->path);
  if (p->last_write_time == last_write_time)
    return false;

  p->last_write_time = last_write_time;
  if (p->last_write_time == -1)
    return false;

  return true;
}

SF_CORE_API
SfFileMonitor *
sf_file_monitor_new (void)
{
  SfFileMonitor *monitor = (SfFileMonitor *) sf_malloc (sizeof (SfFileMonitor));

  sf_linked_list_init (&monitor->watched_files);

  monitor->last_check_time = 0;

  return monitor;
}

SF_CORE_API
void
sf_file_monitor_free (SfFileMonitor *monitor)
{
  SfLinkedListItem *p = monitor->watched_files.next;
  while (p != &monitor->watched_files) {
    SfLinkedListItem *next = p->next;
    sf_watched_file_free ((SfWatchedFile *) p);
    p = next;
  }

  sf_free (monitor);
}

SF_CORE_API
bool
sf_file_monitor_add_watch (SfFileMonitor *monitor,
                           const char *path,
                           SfFileMonitorCallback callback,
                           void *context)
{
  SfWatchedFile *file = sf_watched_file_new (path, callback, context);

  sf_watched_file_check_changes (file);

  sf_linked_list_add_after (&monitor->watched_files, &file->list_item);

  return true;
}

SF_CORE_API
bool
sf_file_monitor_poll (SfFileMonitor *monitor)
{
  SfLinkedListItem *p;
  bool ret = false;
  uint64_t now = sf_time_get_tick_count ();

  /* Add a minimum of 1 second delay between checking the files. */
  if (now - monitor->last_check_time <= 1000)
    return false;
  monitor->last_check_time = now;

  for (p = monitor->watched_files.next; p != &monitor->watched_files; p = p->next) {
    SfWatchedFile *file = (SfWatchedFile *) p;

    if (sf_watched_file_check_changes (file)) {
      file->callback (monitor, file->path, file->context);
      ret = true;
    }
  }

  return ret;
}
