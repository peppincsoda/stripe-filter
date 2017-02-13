#include <sfcore/sf_memory.h>
#include <sfcore/sf_glib_memory.h>

#include <assert.h>
#include <string.h>

SF_CORE_API
void
sf_move_from_gstring (char **ppdest, const gchar **ppsrc)
{
  assert (ppdest != NULL);
  assert (ppsrc != NULL);

  if (*ppsrc == NULL) {
    g_clear_pointer (ppdest, g_free);
  } else {
    size_t len = strlen (*ppsrc);

    if (*ppdest != NULL)
      *ppdest = (char *) sf_realloc (*ppdest, len + 1);
    else
      *ppdest = (char *) sf_malloc (len + 1);
    memcpy (*ppdest, *ppsrc, len + 1);

    g_clear_pointer (ppsrc, g_free);
  }
}

SF_CORE_API
void
sf_move_from_gbuffer (void **ppdest, const void **ppsrc, size_t size)
{
  assert (ppdest != NULL);
  assert (ppsrc != NULL);

  if (*ppsrc == NULL) {
    g_clear_pointer (ppdest, g_free);
  } else {
    if (*ppdest != NULL)
      *ppdest = (char *) sf_realloc (*ppdest, size);
    else
      *ppdest = (char *) sf_malloc (size);
    memcpy (*ppdest, *ppsrc, size);

    g_clear_pointer (ppsrc, g_free);
  }
}
