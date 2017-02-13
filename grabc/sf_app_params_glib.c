#include "sf_app_params_glib.h"

void
sf_app_params_load_from_g_key_file (SfAppParams *p,
                                    GKeyFile *key_file,
                                    const char *group_name)
{
  /* Reset all parameters to default values. */
  sf_app_params_destroy (p);
  sf_app_params_init (p);

  {
    char *str = NULL;
    if (sf_key_file_get_string (key_file, group_name, "output", &str)) {
      SfAppOutput output;
      if (!sf_app_output_from_str (str, &output)) {
        sf_log_warning ("While reading %s/%s: %s", group_name, "output", "Value must be one of 'none', 'stdout' or 'serial'");
      } else {
        p->output = output;
      }
      sf_clear_pointer (&str, sf_free);
    }
  }

  {
    int i;

    if (sf_key_file_get_integer_in_range (key_file, group_name, "modbus-slave-address", 0, 247, &i))
      p->modbus_slave_address = (uint8_t) i;
    if (sf_key_file_get_integer (key_file, group_name, "modbus-register-address", &i))
      p->modbus_register_address = (uint16_t) i;
  }
}
