#include <sfcore/sf_memory.h>

#include <string.h>

SF_CORE_API
char *
sf_strcpy (const char *s)
{
  size_t len = strlen (s);
  char *ret = (char *) sf_malloc (len + 1);
  memcpy (ret, s, len + 1);
  return ret;
}
