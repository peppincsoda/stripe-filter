#ifndef _SF_GLIB_KEY_FILE_H_
#define _SF_GLIB_KEY_FILE_H_

#include  <glib-object.h>

#include "sf_bool.h"

/* Output parameter `value` is changed only if reading the value was successful. */
bool
sf_key_file_get_boolean (GKeyFile *key_file,
                         const char *group_name,
                         const char *key,
                         bool *value);

/* Output parameter `value` is changed only if reading the value was successful
   and it lies within the specified range. */
bool
sf_key_file_get_double_in_range (GKeyFile *key_file,
                                 const char *group_name,
                                 const char *key,
                                 double min_value,
                                 double max_value,
                                 double *value);

/* Output parameter `value` is changed only if reading the value was successful. */
bool
sf_key_file_get_integer (GKeyFile *key_file,
                         const char *group_name,
                         const char *key,
                         int *value);

/* Output parameter `value` is changed only if reading the value was successful
   and it lies within the specified range. */
bool
sf_key_file_get_integer_in_range (GKeyFile *key_file,
                                  const char *group_name,
                                  const char *key,
                                  int min_value,
                                  int max_value,
                                  int *value);

/* Output parameter `value` is changed only if reading the value was successful.
   `*value` has to be freed with sf_free if it is no longer needed. */
bool
sf_key_file_get_string (GKeyFile *key_file,
                        const char *group_name,
                        const char *key,
                        char **value);

#endif  //  _SF_GLIB_KEY_FILE_H_
