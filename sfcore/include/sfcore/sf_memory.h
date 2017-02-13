#ifndef _SF_MEMORY_H_
#define _SF_MEMORY_H_

#include <sfcore/sf_core_header.h>

#ifdef _WIN32
#  define _CRTDBG_MAP_ALLOC
#  include <crtdbg.h>
#  ifndef _DEBUG
#    include <malloc.h>
#  endif
#else
#  include <malloc.h>
#endif

#define sf_malloc(size)      malloc((size))
#define sf_realloc(p, size)  realloc((p), (size))
#define sf_free(p)           free((p))

#define sf_clear_pointer(pp, free_fn) { if (*(pp) != NULL) { free_fn (*(pp)); *(pp) = NULL; } }

#define ARRAY_SIZE(arr)  (sizeof(arr) / sizeof((arr)[0]))

/* The returned string should be freed with sf_free. */
SF_CORE_API
char *
sf_strcpy (const char *s);

#endif  //  _SF_MEMORY_H_
