#include "sf_memory.h"
#include "sf_serial_params.h"
#include "sf_string.h"

bool
sf_serial_parity_from_str (const char *str, SfSerialParity *parity)
{
  if (sf_strcasecmp (str, "none") == 0) {
    *parity = SfSerialParity_None;
  } else if (sf_strcasecmp (str, "even") == 0) {
    *parity = SfSerialParity_Even;
  } else if (sf_strcasecmp (str, "odd") == 0) {
    *parity = SfSerialParity_Odd;
  } else {
    return false;
  }
  return true;
}

SfSerialParams *
sf_serial_params_new (void)
{
  SfSerialParams *p = (SfSerialParams *) sf_malloc (sizeof (SfSerialParams));

  sf_serial_params_init (p);

  return p;
}

void
sf_serial_params_free (SfSerialParams *p)
{
  sf_serial_params_destroy (p);

  sf_free (p);
}

void
sf_serial_params_init (SfSerialParams *p)
{
  p->device_file = NULL;
  p->baud_rate = 19200;
  p->parity = SfSerialParity_Even;
  p->data_bits = 8;
  p->stop_bits = 1;
}

void
sf_serial_params_destroy (SfSerialParams *p)
{
  sf_clear_pointer (&p->device_file, sf_free);
}
