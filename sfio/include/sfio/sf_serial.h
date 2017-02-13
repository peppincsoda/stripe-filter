#ifndef _SF_SERIAL_H_
#define _SF_SERIAL_H_

#include <sfio/sf_io_header.h>
#include <sfio/sf_serial_params.h>
#include <sfcore.h>

typedef struct _SfSerial SfSerial;

SF_IO_API
SfSerial *
sf_serial_new (void);

SF_IO_API
void
sf_serial_free (SfSerial *p);

SF_IO_API
bool
sf_serial_open_port (SfSerial *p, SfSerialParams *params);

SF_IO_API
void
sf_serial_close_port (SfSerial *p);

SF_IO_API
bool
sf_serial_write (SfSerial *p,
                 const void *buffer,
                 int buffer_size);

#endif  //  _SF_SERIAL_H_
