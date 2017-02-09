#include "sf_algorithm.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sf_memory.h"

static int *
partition (int *first, int *last, int pivot)
{
  int tmp;

  /* returns the first item that is not <= than pivot */
  while (first != last) {
    while (*first <= pivot) {
      first++;
      if (first == last)
        return first;
    }
    do {
      last--;
      if (first == last)
        return first;
    } while (*last > pivot);
    tmp = *first;
    *first = *last;
    *last = tmp;
    first++;
  }
  return first;
}

static int
qselect (int *first, int *last, int *n)
{
  int *p;
  int tmp;

  for (;;) {
    if (last - first == 1) {
      assert (first == n);
      return *first;
    }

    /* the first item of the current slice is the pivot */
    p = partition (first, last, *first);
    p--;
    /* p is the last item that is <= than *first */
    tmp = *first;
    *first = *p;
    *p = tmp;

    if (p == n) {
      /* the selected item is just the pivot */
      return *n;
    } else if (p < n) {
      /* the selected item is greater than the pivot */
      first = p + 1;
      n = first + (n - p - 1);
    } else {
      /* the selected item is less than the pivot */
      last = p;
    }
  }
}

//void
//sf_filter_conv (SfFilterParams *p,
//                const int *kernel,
//                int k_size,
//                int k_sum)
//{
//  const uint8_t *p_src;
//  uint8_t *p_dest;
//  int half;
//  int f_width;
//  int row, col, k_row, k_col;
//  const int *p_kernel;
//  const uint8_t *p_src_k;
//  int sum;
//  int r;
//
//  half = (k_size - 1) / 2;
//  f_width = p->width - 2 * half;
//
//  assert (p->src != p->dest);
//  assert (p->width <= p->src_stride);
//  assert (f_width <= p->dest_stride);
//  assert (k_size >= 3 && (k_size % 2) == 1);
//
//  p_src = p->src;
//  p_dest = p->dest;
//  for (row = half; row < p->height - half; row++) {
//    for (col = half; col < p->width - half; col++) {
//
//      p_kernel = kernel;
//      p_src_k = p_src;
//      sum = 0;
//      for (k_row = 0; k_row < k_size; k_row++) {
//        for (k_col = 0; k_col < k_size; k_col++) {
//          sum += *p_src_k++ * *p_kernel++;
//        }
//        p_src_k += p->src_stride - k_size;
//      }
//      r = sum / k_sum;
//      r = r < 0 ? -r : r;
//
//      *p_dest = r;
//
//      p_src++;
//      p_dest++;
//    }
//    p_src += p->src_stride - f_width;
//    p_dest += p->dest_stride - f_width;
//  }
//}

void
sf_filter_conv (SfFilterParams *p,
                const int *kernel,
                int k_size,
                int k_sum)
{
  if (k_size == 3) {
    sf_filter_conv3 (p, kernel, k_sum);
  } else if (k_size == 5) {
    sf_filter_conv5 (p, kernel, k_sum);
  } else {
    assert (0);
  }
}

void
sf_filter_conv3 (SfFilterParams *p,
                 const int *kernel,
                 int k_sum)
{
  const uint8_t *p_src;
  uint8_t *p_dest;
  int row, col;
  const uint8_t *p_src_k;
  int sum;
  int r;

  assert (p->src != p->dest);
  assert (p->width <= p->src_stride);
  assert (p->width - 2 <= p->dest_stride);

  p_src = p->src;
  p_dest = p->dest;
  for (row = 1; row < p->height - 1; row++) {
    for (col = 1; col < p->width - 1; col++) {

      p_src_k = p_src;
      sum = 0;

      sum += p_src_k[0] * kernel[0];
      sum += p_src_k[1] * kernel[1];
      sum += p_src_k[2] * kernel[2];

      p_src_k += p->src_stride;

      sum += p_src_k[0] * kernel[3];
      sum += p_src_k[1] * kernel[4];
      sum += p_src_k[2] * kernel[5];

      p_src_k += p->src_stride;

      sum += p_src_k[0] * kernel[6];
      sum += p_src_k[1] * kernel[7];
      sum += p_src_k[2] * kernel[8];

      r = sum / k_sum;
      r = r < 0 ? -r : r;

      *p_dest = r;

      p_src++;
      p_dest++;
    }
    p_src += p->src_stride - (p->width - 2);
    p_dest += p->dest_stride - (p->width - 2);
  }
}

void
sf_filter_conv5 (SfFilterParams *p,
                 const int *kernel,
                 int k_sum)
{
  const uint8_t *p_src;
  uint8_t *p_dest;
  int row, col;
  const uint8_t *p_src_k;
  int sum;
  int r;

  assert (p->src != p->dest);
  assert (p->width <= p->src_stride);
  assert (p->width - 4 <= p->dest_stride);

  p_src = p->src;
  p_dest = p->dest;
  for (row = 2; row < p->height - 2; row++) {
    for (col = 2; col < p->width - 2; col++) {

      p_src_k = p_src;
      sum = 0;

      sum += p_src_k[0] * kernel[0];
      sum += p_src_k[1] * kernel[1];
      sum += p_src_k[2] * kernel[2];
      sum += p_src_k[3] * kernel[3];
      sum += p_src_k[4] * kernel[4];

      p_src_k += p->src_stride;

      sum += p_src_k[0] * kernel[5];
      sum += p_src_k[1] * kernel[6];
      sum += p_src_k[2] * kernel[7];
      sum += p_src_k[3] * kernel[8];
      sum += p_src_k[4] * kernel[9];

      p_src_k += p->src_stride;

      sum += p_src_k[0] * kernel[10];
      sum += p_src_k[1] * kernel[11];
      sum += p_src_k[2] * kernel[12];
      sum += p_src_k[3] * kernel[13];
      sum += p_src_k[4] * kernel[14];

      p_src_k += p->src_stride;

      sum += p_src_k[0] * kernel[15];
      sum += p_src_k[1] * kernel[16];
      sum += p_src_k[2] * kernel[17];
      sum += p_src_k[3] * kernel[18];
      sum += p_src_k[4] * kernel[19];

      p_src_k += p->src_stride;

      sum += p_src_k[0] * kernel[20];
      sum += p_src_k[1] * kernel[21];
      sum += p_src_k[2] * kernel[22];
      sum += p_src_k[3] * kernel[23];
      sum += p_src_k[4] * kernel[24];

      r = sum / k_sum;
      r = r < 0 ? -r : r;
      
      *p_dest = r;

      p_src++;
      p_dest++;
    }
    p_src += p->src_stride - (p->width - 4);
    p_dest += p->dest_stride - (p->width - 4);
  }
}

static int
sf_filter_find_color (const uint8_t *src_first,
                      const uint8_t *src_last,
                      uint8_t color)
{
  const uint8_t *p_src;

  if (src_first < src_last) {
    /* search from left to right */
    for (p_src = src_first; p_src < src_last; p_src++) {
      if (*p_src == color)
        return p_src - src_first;
    }
  } else {
    /* search from right to left */
    for (p_src = src_first - 1; p_src >= src_last; p_src--) {
      if (*p_src == color)
        return p_src - src_last;
    }
  }

  return -1;
}

int
sf_filter_find_singleline (SfFilterInParams *p,
                           uint8_t color,
                           bool right_to_left)
{
  assert (p->width <= p->stride);

  if (right_to_left)
    return sf_filter_find_color (p->src + p->width,
      p->src, color);
  else
    return sf_filter_find_color (p->src,
      p->src + p->width, color);
}

int
sf_filter_find_multiline (SfFilterInParams *p,
                          uint8_t color,
                          bool right_to_left)
{
  int row;
  const uint8_t *p_src;
  int *col_arr;
  int col_arr_last;
  int ret;

  assert (p->width <= p->stride);

  col_arr = (int *) sf_malloc (p->height * sizeof (int));
  col_arr_last = 0;

  p_src = p->src;
  for (row = 0; row < p->height; row++) {
    int col = sf_filter_find_color (
      right_to_left ? p_src + p->width : p_src,
      right_to_left ? p_src : p_src + p->width,
      color);
    if (col != -1)
      col_arr[col_arr_last++] = col;
    p_src += p->stride;
  }

  if (col_arr_last > 0) {
    if ((col_arr_last % 2) == 0) {
      int med0, med1;
      med0 = qselect (col_arr, &col_arr[col_arr_last], &col_arr[(col_arr_last / 2) - 1]);
      med1 = qselect (col_arr, &col_arr[col_arr_last], &col_arr[col_arr_last / 2]);
      ret = (med0 + med1) / 2;
    } else {
      ret = qselect (col_arr, &col_arr[col_arr_last], &col_arr[col_arr_last / 2]);
    }
  } else {
    ret = -1;
  }

  sf_free (col_arr);
  return ret;
}

void
sf_filter_compute_gaussian_kernel (double sigma,
                                   int k_size,
                                   int *kernel,
                                   int *k_sum)
{
  int row, col;
  int half;
  double *d_kernel;
  int i;

  assert (k_size >= 3 && (k_size % 2) == 1);
  assert (kernel != NULL);
  assert (k_sum != NULL);

  d_kernel = (double *) sf_malloc (k_size*k_size * sizeof(double));

  half = (k_size - 1) / 2;
  for (row = 0; row < k_size; row++) {
    for (col = 0; col < k_size; col++) {
      double x = row - half;
      double y = col - half;
      double r = sqrt (x*x + y*y);

      if (r <= (double) half) {
        d_kernel[row * k_size + col] =
          1.0 / (sqrt (2.0 * M_PI) * sigma) * exp (-r*r / 2.0*sigma*sigma);
      } else {
        d_kernel[row * k_size + col] = 0.0;
      }
    }
  }

  *k_sum = 0;
  for (i = 0; i < k_size*k_size; i++) {
    kernel[i] = (int) (d_kernel[i] * 256.0);
    *k_sum += kernel[i];
  }

  sf_free (d_kernel);
}

//void
//sf_filter_median (SfFilterParams *p,
//                  int k_size)
//{
//  int half;
//  int row, col;
//  int k_row;
//  const uint8_t *p_src, *p_src_k;
//  uint8_t *p_dest;
//  int f_width;
//  uint8_t *arr, *p_arr;
//  int k_sqr;
//  int k_middle;
//
//  half = (k_size - 1) / 2;
//  f_width = p->width - 2 * half;
//
//  assert (p->src != p->dest);
//  assert (p->width <= p->src_stride);
//  assert (p->width - 2 * half <= p->dest_stride);
//  assert (k_size >= 3 && (k_size % 2) == 1);
//
//  k_sqr = k_size * k_size;
//  k_middle = (k_sqr - 1) / 2;
//  arr = (uint8_t *) sf_malloc (k_sqr * sizeof (uint8_t));
//
//  p_src = p->src;
//  p_dest = p->dest;
//  for (row = half; row < p->height - half; row++) {
//    for (col = half; col < p->width - half; col++) {
//      p_arr = arr;
//      p_src_k = p_src;
//      for (k_row = 0; k_row < k_size; ++k_row) {
//        memcpy (p_arr, p_src_k, k_size);
//        p_arr += k_size;
//        p_src_k += p->src_stride;
//      }
//
//      *p_dest = qselect (&arr[0], &arr[k_sqr], &arr[k_middle]);
//
//      p_src++;
//      p_dest++;
//    }
//
//    p_src += p->src_stride - f_width;
//    p_dest += p->dest_stride - f_width;
//  }
//
//  sf_free (arr);
//}

void
sf_filter_median (SfFilterParams *p,
                  int k_size)
{
  if (k_size == 3) {
    sf_filter_median3 (p);
  } else if (k_size == 5) {
    sf_filter_median5 (p);
  } else {
    assert (0);
  }
}

/* Median filters from: http://ndevilla.free.fr/median/median.pdf */

#define PIX_SWAP(a, b) { uint8_t temp = (a); (a) = (b); (b) = temp; }
#define PIX_SORT(a, b) { if ((a) > (b)) PIX_SWAP((a),(b)); }

void
sf_filter_median3 (SfFilterParams *p)
{
  const uint8_t *p_src;
  uint8_t *p_dest;
  int row;
  int col;
  uint8_t px[9];

  assert (p->src != p->dest);
  assert (p->width <= p->src_stride);
  assert (p->width - 2 <= p->dest_stride);

  p_src = p->src;
  p_dest = p->dest;
  for (row = 1; row < p->height - 1; row++) {
    for (col = 1; col < p->width - 1; col++) {
      const uint8_t *p_src_k = p_src;
      px[0] = p_src_k[0];
      px[1] = p_src_k[1];
      px[2] = p_src_k[2];
      p_src_k += p->src_stride;
      px[3] = p_src_k[0];
      px[4] = p_src_k[1];
      px[5] = p_src_k[2];
      p_src_k += p->src_stride;
      px[6] = p_src_k[0];
      px[7] = p_src_k[1];
      px[8] = p_src_k[2];

      PIX_SORT(px[1],px[2]); PIX_SORT(px[4],px[5]); PIX_SORT(px[7],px[8]);
      PIX_SORT(px[0],px[1]); PIX_SORT(px[3],px[4]); PIX_SORT(px[6],px[7]);
      PIX_SORT(px[1],px[2]); PIX_SORT(px[4],px[5]); PIX_SORT(px[7],px[8]);
      PIX_SORT(px[0],px[3]); PIX_SORT(px[5],px[8]); PIX_SORT(px[4],px[7]);
      PIX_SORT(px[3],px[6]); PIX_SORT(px[1],px[4]); PIX_SORT(px[2],px[5]);
      PIX_SORT(px[4],px[7]); PIX_SORT(px[4],px[2]); PIX_SORT(px[6],px[4]);
      PIX_SORT(px[4],px[2]);

      *p_dest = px[4];

      p_src++;
      p_dest++;
    }

    p_src += p->src_stride - (p->width - 2);
    p_dest += p->dest_stride - (p->width - 2);
  }
}

void
sf_filter_median5 (SfFilterParams *p)
{
  const uint8_t *p_src;
  uint8_t *p_dest;
  int row;
  int col;
  uint8_t px[25];

  assert (p->src != p->dest);
  assert (p->width <= p->src_stride);
  assert (p->width - 4 <= p->dest_stride);

  p_src = p->src;
  p_dest = p->dest;
  for (row = 2; row < p->height - 2; row++) {
    for (col = 2; col < p->width - 2; col++) {
      const uint8_t *p_src_k = p_src;
      px[0] = p_src_k[0];
      px[1] = p_src_k[1];
      px[2] = p_src_k[2];
      px[3] = p_src_k[3];
      px[4] = p_src_k[4];
      p_src_k += p->src_stride;
      px[5] = p_src_k[0];
      px[6] = p_src_k[1];
      px[7] = p_src_k[2];
      px[8] = p_src_k[3];
      px[9] = p_src_k[4];
      p_src_k += p->src_stride;
      px[10] = p_src_k[0];
      px[11] = p_src_k[1];
      px[12] = p_src_k[2];
      px[13] = p_src_k[3];
      px[14] = p_src_k[4];
      p_src_k += p->src_stride;
      px[15] = p_src_k[0];
      px[16] = p_src_k[1];
      px[17] = p_src_k[2];
      px[18] = p_src_k[3];
      px[19] = p_src_k[4];
      p_src_k += p->src_stride;
      px[20] = p_src_k[0];
      px[21] = p_src_k[1];
      px[22] = p_src_k[2];
      px[23] = p_src_k[3];
      px[24] = p_src_k[4];

      PIX_SORT(px[0],px[1]);   PIX_SORT(px[3],px[4]);   PIX_SORT(px[2],px[4]);
      PIX_SORT(px[2],px[3]);   PIX_SORT(px[6],px[7]);   PIX_SORT(px[5],px[7]);
      PIX_SORT(px[5],px[6]);   PIX_SORT(px[9],px[10]);  PIX_SORT(px[8],px[10]);
      PIX_SORT(px[8],px[9]);   PIX_SORT(px[12],px[13]); PIX_SORT(px[11],px[13]);
      PIX_SORT(px[11],px[12]); PIX_SORT(px[15],px[16]); PIX_SORT(px[14],px[16]);
      PIX_SORT(px[14],px[15]); PIX_SORT(px[18],px[19]); PIX_SORT(px[17],px[19]);
      PIX_SORT(px[17],px[18]); PIX_SORT(px[21],px[22]); PIX_SORT(px[20],px[22]);
      PIX_SORT(px[20],px[21]); PIX_SORT(px[23],px[24]); PIX_SORT(px[2],px[5]);
      PIX_SORT(px[3],px[6]);   PIX_SORT(px[0],px[6]);   PIX_SORT(px[0],px[3]);
      PIX_SORT(px[4],px[7]);   PIX_SORT(px[1],px[7]);   PIX_SORT(px[1],px[4]);
      PIX_SORT(px[11],px[14]); PIX_SORT(px[8],px[14]);  PIX_SORT(px[8],px[11]);
      PIX_SORT(px[12],px[15]); PIX_SORT(px[9],px[15]);  PIX_SORT(px[9],px[12]);
      PIX_SORT(px[13],px[16]); PIX_SORT(px[10],px[16]); PIX_SORT(px[10],px[13]);
      PIX_SORT(px[20],px[23]); PIX_SORT(px[17],px[23]); PIX_SORT(px[17],px[20]);
      PIX_SORT(px[21],px[24]); PIX_SORT(px[18],px[24]); PIX_SORT(px[18],px[21]);
      PIX_SORT(px[19],px[22]); PIX_SORT(px[8],px[17]);  PIX_SORT(px[9],px[18]);
      PIX_SORT(px[0],px[18]);  PIX_SORT(px[0],px[9]);   PIX_SORT(px[10],px[19]);
      PIX_SORT(px[1],px[19]);  PIX_SORT(px[1],px[10]);  PIX_SORT(px[11],px[20]);
      PIX_SORT(px[2],px[20]);  PIX_SORT(px[2],px[11]);  PIX_SORT(px[12],px[21]);
      PIX_SORT(px[3],px[21]);  PIX_SORT(px[3],px[12]);  PIX_SORT(px[13],px[22]);
      PIX_SORT(px[4],px[22]);  PIX_SORT(px[4],px[13]);  PIX_SORT(px[14],px[23]);
      PIX_SORT(px[5],px[23]);  PIX_SORT(px[5],px[14]);  PIX_SORT(px[15],px[24]);
      PIX_SORT(px[6],px[24]);  PIX_SORT(px[6],px[15]);  PIX_SORT(px[7],px[16]);
      PIX_SORT(px[7],px[19]);  PIX_SORT(px[13],px[21]); PIX_SORT(px[15],px[23]);
      PIX_SORT(px[7],px[13]);  PIX_SORT(px[7],px[15]);  PIX_SORT(px[1],px[9]);
      PIX_SORT(px[3],px[11]);  PIX_SORT(px[5],px[17]);  PIX_SORT(px[11],px[17]);
      PIX_SORT(px[9],px[17]);  PIX_SORT(px[4],px[10]);  PIX_SORT(px[6],px[12]);
      PIX_SORT(px[7],px[14]);  PIX_SORT(px[4],px[6]);   PIX_SORT(px[4],px[7]);
      PIX_SORT(px[12],px[14]); PIX_SORT(px[10],px[14]); PIX_SORT(px[6],px[7]);
      PIX_SORT(px[10],px[12]); PIX_SORT(px[6],px[10]);  PIX_SORT(px[6],px[17]);
      PIX_SORT(px[12],px[17]); PIX_SORT(px[7],px[17]);  PIX_SORT(px[7],px[10]);
      PIX_SORT(px[12],px[18]); PIX_SORT(px[7],px[12]);  PIX_SORT(px[10],px[18]);
      PIX_SORT(px[12],px[20]); PIX_SORT(px[10],px[20]); PIX_SORT(px[10],px[12]);

      *p_dest = px[12];

      p_src++;
      p_dest++;
    }

    p_src += p->src_stride - (p->width - 4);
    p_dest += p->dest_stride - (p->width - 4);
  }
}

#undef PIX_SORT
#undef PIX_SWAP

void
sf_filter_threshold (SfFilterParams *p,
                     int thr,
                     uint8_t val0,
                     uint8_t val1)
{
  int row, col;
  const uint8_t *p_src;
  uint8_t *p_dest;

  p_src = p->src;
  p_dest = p->dest;
  for (row = 0; row < p->height; row++) {
    for (col = 0; col < p->width; col++)
      *p_dest++ = (*p_src++ >= thr) ? val1 : val0;

    p_src += p->src_stride - p->width;
    p_dest += p->dest_stride - p->width;
  }
}

void
sf_filter_copy (SfFilterParams *p)
{
  int row, col;
  const uint8_t *p_src;
  uint8_t *p_dest;

  p_src = p->src + (p->height - 1) * p->src_stride;
  p_dest = p->dest + (p->height - 1) * p->dest_stride;
  for (row = 0; row < p->height; row++) {
    for (col = 0; col < p->width; col++)
      memmove (p_dest, p_src, p->width);

    p_src -= p->src_stride;
    p_dest -= p->dest_stride;
  }
}

void
sf_filter_compute_histogram (SfFilterInParams *p,
                             int *histogram,
                             int h_len)
{
  int row, col;

  assert (h_len == 256);

  memset (histogram, 0, h_len * sizeof (int));
  for (row = 0; row < p->height; row++) {
    for (col = 0; col < p->width; col++)
      histogram[*p->src++]++;

    p->src += p->stride - p->width;
  }
}

int
sf_filter_find_otsu_threshold (const int *histogram,
                               int h_len,
                               int num_pixels)
{
  int i;
  int t;
  int t_max;
  double p_sum_0, p_sum_1, mu_num_0, mu_num_1;
  double var_b_max;

  /* initialize intermediates */
  p_sum_0 = 0;
  p_sum_1 = num_pixels;

  mu_num_0 = 0;
  mu_num_1 = 0;
  for (i = 0; i < h_len; i++)
    mu_num_1 += i * histogram[i];

  /* threshold lays between t and t+1 */
  var_b_max = 0;
  for (t = 0; t < h_len - 1; t++) {
    double mu_0, mu_1;
    double var_b; /* between class variance */

    p_sum_0 += histogram[t];
    p_sum_1 -= histogram[t];

    mu_num_0 += t * histogram[t];
    mu_num_1 -= t * histogram[t];

    mu_0 = mu_num_0 / p_sum_0;
    mu_1 = mu_num_1 / p_sum_1;
    var_b = (p_sum_0 / num_pixels) * (p_sum_1 / num_pixels) * (mu_0 - mu_1) * (mu_0 - mu_1);
    if (var_b_max < var_b) {
      var_b_max = var_b;
      t_max = t;
    }
  }

  return t_max;
}

/*
 * A Bresenham implementation from www.falloutsoftware.com
 */
void
sf_filter_draw_line (SfFilterOutParams *params,
                     int x0, int y0,
                     int x1, int y1,
                     uint8_t color)
{
  int dx, dy;
  int xadd, yadd;
  int dx2, dy2;
  int err;
  int i;
  uint8_t *p;

  assert (0 <= x0 && x0 < params->width);
  assert (0 <= x1 && x1 < params->width);
  assert (0 <= y0 && y0 < params->height);
  assert (0 <= y1 && y1 < params->height);

  p = params->dest + y0 * params->stride + x0;

  dx = x1 - x0;
  dy = y1 - y0;

  if (dx >= 0) {
    xadd = 1;
  } else {
    xadd = -1;
    dx = -dx;
  }
  if (dy >= 0) {
    yadd = params->stride;
  } else {
    yadd = -params->stride;
    dy = -dy;
  }

  dx2 = dx * 2;
  dy2 = dy * 2;

  if (dx > dy) {
    err = dy2 - dx;
    for (i = 0; i <= dx; i++) {
      *p = color;
      if (err >= 0) {
        err -= dx2;
        p += yadd;
      }
      err += dy2;
      p += xadd;
    }
  } else {
    err = dx2 - dy;
    for (i = 0; i <= dy; i++) {
      *p = color;
      if (err >= 0) {
        err -= dy2;
        p += xadd;
      }
      err += dx2;
      p += yadd;
    }
  }
}

void
sf_filter_draw_histogram (SfFilterOutParams *params,
                          const int *histogram,
                          int h_len,
                          int threshold,
                          int x, int y,
                          int width,
                          uint8_t bg_color, uint8_t fg_color, uint8_t th_color)
{
  uint8_t *dest;
  int col;
  int i;
  int max;
  int bar_width;

  assert (params->width <= params->stride);
  assert (x + width <= params->width);
  assert (y + h_len + 2 <= params->height);

  max = 0;
  for (i = 0; i < h_len; i++)
    if (max < histogram[i])
      max = histogram[i];

  dest = params->dest + y * params->stride + x;
  for (col = 0; col < width; col++)
    *dest++ = fg_color; /* frame */
  dest += params->stride - width;

  for (i = 0; i < h_len; i++) {
    if (i == threshold) {
      for (col = 0; col < width; col++)
        *dest++ = th_color; /* threshold */
    } else {
      bar_width = histogram[i] * (width - 2) / max;
      *dest++ = fg_color; /* frame */
      for (col = 0; col < (width - 2) - bar_width; col++)
        *dest++ = bg_color;
      for (col = 0; col < bar_width; col++)
        *dest++ = fg_color;
      *dest++ = fg_color; /* frame */
    }
    dest += params->stride - width;
  }

  for (col = 0; col < width; col++)
    *dest++ = fg_color; /* frame */
}

void
sf_filter_draw_histogram_horizontal (SfFilterOutParams *params,
                                     const int *histogram,
                                     int h_len,
                                     int threshold,
                                     int x, int y,
                                     int height,
                                     uint8_t bg_color, uint8_t fg_color, uint8_t th_color)
{
  int i;
  int max;

  assert (params->width <= params->stride);

  /* Do not draw anything if it does not fit into the screen. */
  if (x + h_len + 2 >= params->width ||
      y + height >= params->height)
    return;

  max = 0;
  for (i = 0; i < h_len; i++)
    if (max < histogram[i])
      max = histogram[i];

  /* Draw frame. */
  sf_filter_draw_line (params, x, y, x + h_len + 1, y, fg_color);
  sf_filter_draw_line (params, x, y + height - 1, x + h_len + 1, y + height - 1, fg_color);
  sf_filter_draw_line (params, x, y, x, y + height - 1, fg_color);
  sf_filter_draw_line (params, x + h_len + 1, y, x + h_len + 1, y + height - 1, fg_color);

  /* Draw histogram. */
  for (i = 0; i < h_len; i++) {
    int bar_height = histogram[i] * (height - 2) / max;
    sf_filter_draw_line (params, x + i + 1, y + 1, x + i + 1, y + height - bar_height - 1, bg_color);
    sf_filter_draw_line (params, x + i + 1, y + height - bar_height - 1, x + i + 1, y + height - 1, fg_color);
  }

  /* Draw scale. */
  for (i = 1; i < 8; i++)
    sf_filter_draw_line (params, x + i * 32 + 1, y + height - 2, x + i * 32 + 1, y + height - 1, th_color);

  /* Draw threshold line. */
  sf_filter_draw_line (params, x + threshold + 1, y + 1, x + threshold + 1, y + height - 1, th_color);
}

int
sf_binary_search (int *arr, int len, int item)
{
  int lower = 0;
  int upper = len - 1;

  while (lower <= upper) {
    int middle = (lower + upper) / 2;

    if (arr[middle] == item)
      return middle;
    else if (arr[middle] < item)
      lower = middle + 1;
    else
      upper = middle - 1;
  }

  return -1;
}
