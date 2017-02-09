#ifndef _SF_SERIAL_PARAMS_H_
#define _SF_SERIAL_PARAMS_H_

#include "sf_bool.h"

typedef enum _SfSerialParity SfSerialParity;

enum _SfSerialParity
{
  SfSerialParity_None,
  SfSerialParity_Even,
  SfSerialParity_Odd,
};

typedef struct _SfSerialParams SfSerialParams;

struct _SfSerialParams
{
  char *device_file;      /* Windows: \\.\COM1   Linux: /dev/ttyUSB0 */
  int baud_rate;          /* Speed: 9600, 19200, 115200, etc. */
  SfSerialParity parity;  /* Parity setting */
  int data_bits;          /* Data bits (7 or 8) */
  int stop_bits;          /* Stop bits (1 or 2) */
};

bool
sf_serial_parity_from_str (const char *str, SfSerialParity *parity);

SfSerialParams *
sf_serial_params_new (void);

void
sf_serial_params_free (SfSerialParams *p);

void
sf_serial_params_init (SfSerialParams *p);

void
sf_serial_params_destroy (SfSerialParams *p);

#endif  //  _SF_SERIAL_PARAMS_H_
