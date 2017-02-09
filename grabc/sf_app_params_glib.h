#ifndef _SF_APP_PARAMS_GLIB_H_
#define _SF_APP_PARAMS_GLIB_H_

#include "sf_app_params.h"

#include <glib-object.h>

void
sf_app_params_load_from_g_key_file (SfAppParams *p,
                                    GKeyFile *key_file,
                                    const char *group_name);

#endif  //  _SF_APP_PARAMS_GLIB_H_
