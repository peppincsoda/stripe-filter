#include "sf_processor_params.h"
#include "sf_memory.h"
#include "sf_string.h"

bool
sf_process_region_from_str (const char *str, SfProcessRegion *region)
{
  if (sf_strcasecmp (str, "none") == 0) {
    *region = SfProcessRegion_None;
  } else if (sf_strcasecmp (str, "left") == 0) {
    *region = SfProcessRegion_Left;
  } else if (sf_strcasecmp (str, "right") == 0) {
    *region = SfProcessRegion_Right;
  } else if (sf_strcasecmp (str, "overlay") == 0) {
    *region = SfProcessRegion_Overlay;
  } else {
    return false;
  }
  return true;
}

SfProcessorParams *
sf_processor_params_new (void)
{
  SfProcessorParams *p = (SfProcessorParams *) sf_malloc (sizeof (SfProcessorParams));

  sf_processor_params_init (p);

  return p;
}

void
sf_processor_params_free (SfProcessorParams *p)
{
  sf_processor_params_destroy (p);

  sf_free (p);
}

void
sf_processor_params_init (SfProcessorParams *p)
{
  p->draw_overlay = false;
  
  p->f_width = 0.9;
  p->f_height = 0.1;
  p->f_border = 1.0;
  
  p->kmed_size = 0;

  p->kconv_data = NULL;
  p->kconv_size = 0;
  p->kconv_sum = 0;

  p->threshold = 0;
  p->s_height = 1;

  p->histogram_region = SfProcessRegion_None;

  p->compute_otsu = false;
  p->dark_object = false;
}

void
sf_processor_params_destroy (SfProcessorParams *p)
{
  sf_clear_pointer (&p->kconv_data, sf_free);
}
