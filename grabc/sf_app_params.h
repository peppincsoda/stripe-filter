#ifndef _SF_APP_PARAMS_H_
#define _SF_APP_PARAMS_H_

#include <sfcore.h>
#include <stdint.h>

typedef enum _SfAppOutput SfAppOutput;

enum _SfAppOutput
{
  SfAppOutput_None,
  SfAppOutput_StdOut,
  SfAppOutput_Serial,
};

bool
sf_app_output_from_str (const char *str, SfAppOutput *output);

typedef struct _SfAppParams SfAppParams;

struct _SfAppParams
{
  SfAppOutput output;

  uint8_t modbus_slave_address;
  uint16_t modbus_register_address;
};

SfAppParams *
sf_app_params_new (void);

void
sf_app_params_free (SfAppParams *p);

void
sf_app_params_init (SfAppParams *p);

void
sf_app_params_destroy (SfAppParams *p);

#endif  //  _SF_APP_PARAMS_H_
