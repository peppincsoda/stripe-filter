#include <sfio/sf_serial_params_glib.h>

#include <sfcore.h>

SF_IO_API
void
sf_serial_params_load_from_g_key_file (SfSerialParams *p,
                                       GKeyFile *key_file,
                                       const gchar *group_name)
{
  /* Reset all parameters to default values. */
  sf_serial_params_destroy (p);
  sf_serial_params_init (p);

  sf_key_file_get_string (key_file, group_name, "device-file", &p->device_file);
  sf_key_file_get_integer (key_file, group_name, "baud-rate", &p->baud_rate);

  {
    char *str = NULL;
    if (sf_key_file_get_string (key_file, group_name, "parity", &str)) {
      SfSerialParity parity;
      if (!sf_serial_parity_from_str (str, &parity)) {
        sf_log_warning ("While reading %s/%s: %s", group_name, "parity", "Value must be one of 'none', 'even' or 'odd'");
      } else {
        p->parity = parity;
      }
      sf_clear_pointer (&str, sf_free);
    }
  }

  sf_key_file_get_integer_in_range (key_file, group_name, "data-bits", 7, 8, &p->data_bits);
  sf_key_file_get_integer_in_range (key_file, group_name, "stop-bits", 1, 2, &p->stop_bits);
}
