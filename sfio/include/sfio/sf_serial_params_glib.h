#ifndef _SF_SERIAL_PARAMS_GLIB_H_
#define _SF_SERIAL_PARAMS_GLIB_H_

#include <sfio/sf_io_header.h>
#include <sfio/sf_serial_params.h>

#include <glib-object.h>

SF_IO_API
void
sf_serial_params_load_from_g_key_file (SfSerialParams *p,
                                       GKeyFile *key_file,
                                       const gchar *group_name);

#endif  //  _SF_SERIAL_PARAMS_GLIB_H_
