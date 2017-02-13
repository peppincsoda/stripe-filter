#ifndef _SF_SERIAL_PARAMS_H_
#define _SF_SERIAL_PARAMS_H_

#include <sfio/sf_io_header.h>
#include <sfcore.h>

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

SF_IO_API
bool
sf_serial_parity_from_str (const char *str, SfSerialParity *parity);

SF_IO_API
SfSerialParams *
sf_serial_params_new (void);

SF_IO_API
void
sf_serial_params_free (SfSerialParams *p);

SF_IO_API
void
sf_serial_params_init (SfSerialParams *p);

SF_IO_API
void
sf_serial_params_destroy (SfSerialParams *p);

#endif  //  _SF_SERIAL_PARAMS_H_
