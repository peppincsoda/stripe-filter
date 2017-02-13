#ifndef _SF_PROCESSOR_PARAMS_H_
#define _SF_PROCESSOR_PARAMS_H_

#include <sfcore.h>

typedef enum _SfProcessRegion SfProcessRegion;

enum _SfProcessRegion
{
  SfProcessRegion_None,
  SfProcessRegion_Left,
  SfProcessRegion_Right,
  SfProcessRegion_Overlay,

  SfNumProcessRegions,
};

typedef struct _SfProcessorParams SfProcessorParams;

struct _SfProcessorParams
{
  bool draw_overlay; /* Draw processing results over the input image. */
  
  double f_width;    /* AOI width between 0.0-1.0. */
  double f_height;   /* AOI height between 0.0-1.0. */
  double f_border;   /* Border size between 0.0-1.0 relative to f_width.
                        Edges are searched only within the border on both sides.
                        For example, a value of 0.5 means that the search from left to right and the search
                        from right to left will go at most to the center of the AOI.
                        A value of 1.0 means that both searches will proceed to the other end of the AOI.
                        (This can be useful if the object is not centered in the image.) */
  
  int kmed_size;     /* Size of the median pre-filter. (0, 3 or 5) */

  int *kconv_data;   /* Convolutional kernel coefficients. */
  int kconv_size;    /* Convolutional kernel size. (0, 3 or 5) */
  int kconv_sum;     /* Divisor of the convolutional kernel. This is usually the sum of all
                        coefficients if all of them are non-negative. */

  int threshold;     /* Threshold value for the binary thresholding. */
  int s_height;      /* Number of rows searched for the edge. If this value is >1,
                        the final result is the median of the individual (valid) results. */

  SfProcessRegion histogram_region; /* The region for which to show the histogram in the image. */

  bool compute_otsu; /* Compute the Otsu threshold using the selected histogram region and save
                        the result into `threshold`. This is done only once right after this
                        value was set to true. (Then it is set to false.) */
  bool dark_object;  /* True if the object is darker than the background, false otherwise.
                        (The edge that will be searched for after thresholding is 0->255
                        if this is value true, and 255->0 if this value is false. */
};

bool
sf_process_region_from_str (const char *str, SfProcessRegion *region);

SfProcessorParams *
sf_processor_params_new (void);

void
sf_processor_params_free (SfProcessorParams *p);

void
sf_processor_params_init (SfProcessorParams *p);

void
sf_processor_params_destroy (SfProcessorParams *p);

#endif  //  _SF_PROCESSOR_PARAMS_H_
