#include "sf_app_params.h"
#include "sf_memory.h"
#include "sf_string.h"

bool
sf_app_output_from_str (const char *str, SfAppOutput *output)
{
  if (sf_strcasecmp (str, "none") == 0) {
    *output = SfAppOutput_None;
  } else if (sf_strcasecmp (str, "stdout") == 0) {
    *output = SfAppOutput_StdOut;
  } else if (sf_strcasecmp (str, "serial") == 0) {
    *output = SfAppOutput_Serial;
  } else {
    return false;
  }
  return true;
}

SfAppParams *
sf_app_params_new (void)
{
  SfAppParams *p = (SfAppParams *) sf_malloc (sizeof (SfAppParams));

  sf_app_params_init (p);

  return p;
}

void
sf_app_params_free (SfAppParams *p)
{
  sf_app_params_destroy (p);

  sf_free (p);
}

void
sf_app_params_init (SfAppParams *p)
{
  p->output = SfAppOutput_None;

  p->modbus_slave_address = 1;
  p->modbus_register_address = 4000;
}

void
sf_app_params_destroy (SfAppParams *p)
{
}
