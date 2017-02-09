#ifndef _SF_GLIB_MEMORY_H_
#define _SF_GLIB_MEMORY_H_

#include <glib-object.h>

/* Turn a Glib allocated string into an sf_* allocated one,
   so that later sf_* function calls can be safely issued on *ppdest. */
void
sf_move_from_gstring (char **ppdest, const gchar **ppsrc);

/* Turn a Glib allocated buffer into an sf_* allocated one,
   so that later sf_* function calls can be safely issued on *ppdest. */
void
sf_move_from_gbuffer (void **ppdest, const void **ppsrc, size_t size);

#endif  //  _SF_GLIB_MEMORY_H_
