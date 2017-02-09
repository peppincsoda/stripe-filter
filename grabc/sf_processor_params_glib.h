#ifndef _SF_PROCESSOR_PARAMS_GLIB_H_
#define _SF_PROCESSOR_PARAMS_GLIB_H_

#include "sf_processor_params.h"

#include <glib-object.h>

void
sf_processor_params_load_from_g_key_file (SfProcessorParams *p,
                                          GKeyFile *key_file,
                                          const gchar *group_name);

#endif  //  _SF_PROCESSOR_PARAMS_GLIB_H_
