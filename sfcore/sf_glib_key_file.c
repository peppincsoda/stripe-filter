#include <sfcore/sf_memory.h>
#include <sfcore/sf_glib_key_file.h>

#include <sfcore/sf_logger.h>
#include <sfcore/sf_glib_memory.h>

SF_CORE_API
bool
sf_key_file_get_boolean (GKeyFile *key_file,
                         const char *group_name,
                         const char *key,
                         bool *value)
{
  gboolean val;
  GError *err = NULL;

  val = g_key_file_get_boolean (key_file, group_name, key, &err);
  if (err != NULL) {
    sf_log_warning ("While reading %s/%s: %s", group_name, key, err->message);
    g_clear_error (&err);
    return false;
  }
  
  *value = val;
  return true;
}

SF_CORE_API
bool
sf_key_file_get_double_in_range (GKeyFile *key_file,
                                 const char *group_name,
                                 const char *key,
                                 double min_value,
                                 double max_value,
                                 double *value)
{
  gdouble val;
  GError *err = NULL;

  val = g_key_file_get_double (key_file, group_name, key, &err);
  if (err != NULL) {
    sf_log_warning ("While reading %s/%s: %s", group_name, key, err->message);
    g_clear_error (&err);
    return false;
  }
  
  if (val < min_value || val > max_value) {
    sf_log_warning ("While reading %s/%s: Value is not between range %lf-%lf", group_name, key, min_value, max_value);
    return false;
  }

  *value = val;
  return true;
}

SF_CORE_API
bool
sf_key_file_get_integer (GKeyFile *key_file,
                         const char *group_name,
                         const char *key,
                         int *value)
{
  gint val;
  GError *err = NULL;

  val = g_key_file_get_integer (key_file, group_name, key, &err);
  if (err != NULL) {
    sf_log_warning ("While reading %s/%s: %s", group_name, key, err->message);
    g_clear_error (&err);
    return false;
  }
  
  *value = val;
  return true;
}

SF_CORE_API
bool
sf_key_file_get_integer_in_range (GKeyFile *key_file,
                                  const char *group_name,
                                  const char *key,
                                  int min_value,
                                  int max_value,
                                  int *value)
{
  gint val;
  GError *err = NULL;

  val = g_key_file_get_integer (key_file, group_name, key, &err);
  if (err != NULL) {
    sf_log_warning ("While reading %s/%s: %s", group_name, key, err->message);
    g_clear_error (&err);
    return false;
  }
  
  if (val < min_value || val > max_value) {
    sf_log_warning ("While reading %s/%s: Value is not between range %d-%d", group_name, key, min_value, max_value);
    return false;
  }

  *value = val;
  return true;
}

SF_CORE_API
bool
sf_key_file_get_string (GKeyFile *key_file,
                        const char *group_name,
                        const char *key,
                        char **value)
{
  gchar *val;
  GError *err = NULL;

  val = g_key_file_get_string (key_file, group_name, key, &err);
  if (err != NULL) {
    sf_log_warning ("While reading %s/%s: %s", group_name, key, err->message);
    g_clear_error (&err);
    g_assert (val == NULL);
    return false;
  }
  
  sf_move_from_gstring (value, (const gchar **) &val);
  return true;
}
