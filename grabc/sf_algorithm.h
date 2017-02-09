#ifndef _SF_ALGORITHM_H_
#define _SF_ALGORITHM_H_

#include "sf_bool.h"

#include <stdint.h>

typedef struct _SfFilterParams SfFilterParams;

struct _SfFilterParams
{
  int width;
  int height;
  
  const uint8_t *src;
  int src_stride;

  uint8_t *dest;
  int dest_stride;
};

typedef struct _SfFilterInParams SfFilterInParams;

struct _SfFilterInParams
{
  int width;
  int height;
  int stride;

  const uint8_t *src;
};

typedef struct _SfFilterOutParams SfFilterOutParams;

struct _SfFilterOutParams
{
  int width;
  int height;
  int stride;

  uint8_t *dest;
};

void
sf_filter_conv (SfFilterParams *p,
                const int *kernel,
                int k_size,
                int k_sum);

void
sf_filter_conv3 (SfFilterParams *p,
                 const int *kernel,
                 int k_sum);

void
sf_filter_conv5 (SfFilterParams *p,
                 const int *kernel,
                 int k_sum);

int
sf_filter_find_singleline (SfFilterInParams *p,
                           uint8_t color,
                           bool right_to_left);

int
sf_filter_find_multiline (SfFilterInParams *p,
                          uint8_t color,
                          bool right_to_left);

void
sf_filter_compute_gaussian_kernel (double sigma,
                                   int k_size,
                                   int *kernel,
                                   int *k_sum);

void
sf_filter_median (SfFilterParams *p,
                  int k_size);

void
sf_filter_median3 (SfFilterParams *p);

void
sf_filter_median5 (SfFilterParams *p);

void
sf_filter_threshold (SfFilterParams *p,
                     int thr,
                     uint8_t val0,
                     uint8_t val1);

void
sf_filter_copy (SfFilterParams *p);

void
sf_filter_compute_histogram (SfFilterInParams *p,
                             int *histogram,
                             int h_len);

int
sf_filter_find_otsu_threshold (const int *histogram,
                               int h_len,
                               int num_pixels);

void
sf_filter_draw_line (SfFilterOutParams *params,
                     int x0, int y0,
                     int x1, int y1,
                     uint8_t color);

void
sf_filter_draw_histogram (SfFilterOutParams *params,
                          const int *histogram,
                          int h_len,
                          int threshold,
                          int x, int y,
                          int width,
                          uint8_t bg_color, uint8_t fg_color, uint8_t th_color);

void
sf_filter_draw_histogram_horizontal (SfFilterOutParams *params,
                                     const int *histogram,
                                     int h_len,
                                     int threshold,
                                     int x, int y,
                                     int height,
                                     uint8_t bg_color, uint8_t fg_color, uint8_t th_color);

int
sf_binary_search (int *arr, int len, int item);

#endif  //  _SF_ALGORITHM_H_
