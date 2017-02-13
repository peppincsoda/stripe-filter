#ifndef _SF_PROCESSOR_H_
#define _SF_PROCESSOR_H_

#include <sfcore.h>
#include "sf_processor_params.h"

typedef struct _SfProcessor SfProcessor;

SfProcessor *
sf_processor_new (void);

void
sf_processor_free (SfProcessor *p);

/* Returns true if the measuring succeeded.
   The thickness of the pipe is in pixels.
 */
bool
sf_processor_process_image (SfProcessor *proc,
                            SfFilterParams *p,
                            SfProcessorParams *params,
                            int *thickness);

#endif  //  _SF_PROCESSOR_H_
