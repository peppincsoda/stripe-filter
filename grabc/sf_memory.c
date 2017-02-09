#include "sf_memory.h"

#include <string.h>

char *
sf_strcpy (const char *s)
{
  size_t len = strlen (s);
  char *ret = (char *) sf_malloc (len + 1);
  memcpy (ret, s, len + 1);
  return ret;
}
