#ifndef _SF_SERIAL_H_
#define _SF_SERIAL_H_

#include "sf_serial_params.h"
#include "sf_bool.h"

typedef struct _SfSerial SfSerial;

SfSerial *
sf_serial_new (void);

void
sf_serial_free (SfSerial *p);

bool
sf_serial_open_port (SfSerial *p, SfSerialParams *params);

void
sf_serial_close_port (SfSerial *p);

bool
sf_serial_write (SfSerial *p,
                 const void *buffer,
                 int buffer_size);

#endif  //  _SF_SERIAL_H_
