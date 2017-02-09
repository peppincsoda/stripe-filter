#include "sf_processor.h"
#include "sf_memory.h"

#include <memory.h>
#include <assert.h>

static void
process_region (SfProcessor *p,
                SfFilterInParams *fparams,
                SfProcessorParams *pparams,
                int region_i,
                int row_first, int row_last,
                int col_first, int col_last,
                bool compute_histogram,
                const uint8_t **out_p,
                int *out_width, int *out_height, int *out_stride,
                int *offset_x, int *offset_y);

struct _SfProcessor
{
  uint8_t *med_buf[SfNumProcessRegions];
  uint8_t *conv_buf[SfNumProcessRegions];
  uint8_t *thr_buf[SfNumProcessRegions];
  
  int med_buf_size[SfNumProcessRegions];
  int conv_buf_size[SfNumProcessRegions];
  int thr_buf_size[SfNumProcessRegions];

  int histogram[256];
};

SfProcessor *
sf_processor_new (void)
{
  SfProcessor *p;

  p = (SfProcessor *) sf_malloc (sizeof (SfProcessor));
  memset (p, 0, sizeof (*p));

  return p;
}

void
sf_processor_free (SfProcessor *p)
{
  int i;

  for (i = 0; i < SfNumProcessRegions; i++) {
    sf_clear_pointer (&p->med_buf[i], sf_free);
    sf_clear_pointer (&p->conv_buf[i], sf_free);
    sf_clear_pointer (&p->thr_buf[i], sf_free);
  }

  sf_free (p);
}

bool
sf_processor_process_image (SfProcessor *proc,
                            SfFilterParams *p,
                            SfProcessorParams *params,
                            int *thickness)
{
  int f_offset;  // total offset of the output image relative to the input image (x and y are the same)
  int row;
  int col_first[SfNumProcessRegions], col_last[SfNumProcessRegions];
  int pos[SfNumProcessRegions];

  const uint8_t *out_p;
  int out_width, out_height, out_stride;
  int offset_x, offset_y;
  int s_height_half;

  SfFilterInParams region_p;
  SfFilterInParams find_p;

  bool ret;

  assert (p != NULL);
  assert (params != NULL);
  assert (p->src != NULL);
  assert (thickness != NULL);

  f_offset = 0;
  if (params->kmed_size > 0)
    f_offset += (params->kmed_size - 1) / 2;
  if (params->kconv_size > 0 && params->kconv_sum != 0)
    f_offset += (params->kconv_size - 1) / 2;

  row = (int) (p->height * 0.5);
  s_height_half = (params->s_height - 1) / 2;

  region_p.width = p->width;
  region_p.height = p->height;
  region_p.stride = p->src_stride;
  region_p.src = p->src;

  /* LEFT REGION */
  col_first[SfProcessRegion_Left] = (int) (p->width * (0.5 - params->f_width * 0.5));
  col_last[SfProcessRegion_Left] = (int) (p->width * (0.5 - params->f_width * 0.5 + params->f_width * params->f_border));
  process_region (proc,
    &region_p,
    params,
    SfProcessRegion_Left,
    row - s_height_half - f_offset, row + s_height_half + f_offset + 1,
    col_first[SfProcessRegion_Left], col_last[SfProcessRegion_Left],
    params->histogram_region == SfProcessRegion_Left,
    &out_p,
    &out_width, &out_height, &out_stride,
    &offset_x, &offset_y);
  assert (out_height == params->s_height);
  find_p.width = out_width;
  find_p.height = out_height;
  find_p.stride = out_stride;
  find_p.src = out_p;
  pos[SfProcessRegion_Left] = sf_filter_find_multiline (&find_p,
    params->dark_object ? 0 : 255,
    false);
  if (pos[SfProcessRegion_Left] != -1)
    pos[SfProcessRegion_Left] = col_first[SfProcessRegion_Left] + offset_x + pos[SfProcessRegion_Left];

  /* RIGHT REGION */
  col_first[SfProcessRegion_Right] = (int) (p->width * (0.5 + params->f_width * 0.5 - params->f_width * params->f_border));
  col_last[SfProcessRegion_Right] = (int) (p->width * (0.5 + params->f_width * 0.5));
  process_region (proc,
    &region_p,
    params,
    SfProcessRegion_Right,
    row - s_height_half - f_offset, row + s_height_half + f_offset + 1,
    col_first[SfProcessRegion_Right], col_last[SfProcessRegion_Right],
    params->histogram_region == SfProcessRegion_Right,
    &out_p,
    &out_width, &out_height, &out_stride,
    &offset_x, &offset_y);
  assert (out_height == params->s_height);
  find_p.width = out_width;
  find_p.height = out_height;
  find_p.stride = out_stride;
  find_p.src = out_p;
  pos[SfProcessRegion_Right] = sf_filter_find_multiline (&find_p,
    params->dark_object ? 0 : 255,
    true);
  if (pos[SfProcessRegion_Right] != -1)
    pos[SfProcessRegion_Right] = col_first[SfProcessRegion_Right] + offset_x + pos[SfProcessRegion_Right];

  if (pos[SfProcessRegion_Left] != -1 && pos[SfProcessRegion_Right] != -1) {
    *thickness = pos[SfProcessRegion_Right] - pos[SfProcessRegion_Left];
    ret = true;
  } else {
    *thickness = 0;
    ret = false;
  }

  if (params->draw_overlay) {
    /* OVERLAY REGION */
    int row_first = (int) (p->height * (0.5 - params->f_height * 0.5));
    int row_last = (int) (p->height * (0.5 + params->f_height * 0.5));

    /* apply filters to the selected region */
    process_region (proc,
      &region_p,
      params,
      SfProcessRegion_Overlay,
      row_first, row_last,
      col_first[SfProcessRegion_Left], col_last[SfProcessRegion_Right],
      params->histogram_region == SfProcessRegion_Overlay,
      &out_p,
      &out_width, &out_height, &out_stride,
      &offset_x, &offset_y);

    /* copy filtered region over the output */
    {
      SfFilterParams copy_p;
      copy_p.width = out_width;
      copy_p.height = out_height;
      copy_p.src = out_p;
      copy_p.src_stride = out_stride;
      copy_p.dest = p->dest + (row_first + offset_y) * p->dest_stride +
        col_first[SfProcessRegion_Left] + offset_x;
      copy_p.dest_stride = p->dest_stride;
      sf_filter_copy (&copy_p);
    }

    /* draw the borders of the region */
    {
      SfFilterOutParams out_params;
      int x[8], y[8];

      out_params.dest = p->dest;
      out_params.width = p->width;
      out_params.height = p->height;
      out_params.stride = p->dest_stride;

      x[0] = x[3] = col_first[SfProcessRegion_Left];
      x[1] = x[2] = col_last[SfProcessRegion_Left];
      x[4] = x[7] = col_first[SfProcessRegion_Right];
      x[5] = x[6] = col_last[SfProcessRegion_Right];
      y[0] = y[1] = y[4] = y[5] = row_first;
      y[3] = y[2] = y[7] = y[6] = row_last;

      /* left frame border */
      sf_filter_draw_line (&out_params, x[0], y[0], x[1], y[1], 100);
      sf_filter_draw_line (&out_params, x[1], y[1], x[2], y[2], 100);
      sf_filter_draw_line (&out_params, x[2], y[2], x[3], y[3], 100);
      sf_filter_draw_line (&out_params, x[3], y[3], x[0], y[0], 100);
      /* left frame centerline */
      sf_filter_draw_line (&out_params,
        x[0], row - s_height_half - 1, x[1], row - s_height_half - 1, 100);
      sf_filter_draw_line (&out_params,
        x[0], row + s_height_half + 1, x[1], row + s_height_half + 1, 100);

      /* right frame border */
      sf_filter_draw_line (&out_params, x[4], y[4], x[5], y[5], 100);
      sf_filter_draw_line (&out_params, x[5], y[5], x[6], y[6], 100);
      sf_filter_draw_line (&out_params, x[6], y[6], x[7], y[7], 100);
      sf_filter_draw_line (&out_params, x[7], y[7], x[4], y[4], 100);
      /* right frame centerline */
      sf_filter_draw_line (&out_params,
        x[4], row - s_height_half - 1, x[5], row - s_height_half - 1, 100);
      sf_filter_draw_line (&out_params,
        x[4], row + s_height_half + 1, x[5], row + s_height_half + 1, 100);

      /* current output of the edge detectors */
      if (pos[SfProcessRegion_Left] == -1)
        sf_filter_draw_line (&out_params, x[0], y[0], x[0], y[3], 255);
      else
        sf_filter_draw_line (&out_params, pos[SfProcessRegion_Left], y[0], pos[SfProcessRegion_Left], y[3], 255);
      if (pos[SfProcessRegion_Right] == -1)
        sf_filter_draw_line (&out_params, x[5], y[4], x[5], y[7], 255);
      else
        sf_filter_draw_line (&out_params, pos[SfProcessRegion_Right], y[4], pos[SfProcessRegion_Right], y[7], 255);

      /* histogram */
      if (params->histogram_region != SfProcessRegion_None) {
        int h_height = (int) (((1.0 - params->f_height) / 2.0) * p->height);
        if (h_height > 100)
          h_height = 100;

        sf_filter_draw_histogram_horizontal (&out_params,
          proc->histogram,
          ARRAY_SIZE (proc->histogram),
          params->threshold,
          10, p->height - h_height,
          h_height - 1,
          0, 100, 255);
      }
    }
  }

  /* compute Otsu threshold for the selected region */
  if (params->compute_otsu) {
    params->compute_otsu = false;
    if (params->histogram_region != SfProcessRegion_None) {
      params->threshold = sf_filter_find_otsu_threshold (proc->histogram,
        ARRAY_SIZE (proc->histogram),
        out_width * out_height);
    }
  }

  return ret;
}

static void
realloc_target_buffer (uint8_t **buf_p, int *buf_size, int new_size)
{
  if (*buf_size != new_size) {
    *buf_p = (uint8_t *) sf_realloc (*buf_p, new_size * sizeof (uint8_t));
    *buf_size = new_size;
  }
}

static void
process_region (SfProcessor *proc,
                SfFilterInParams *p,
                SfProcessorParams *params,
                int region_i,
                int row_first, int row_last,
                int col_first, int col_last,
                bool compute_histogram,
                const uint8_t **out_p,
                int *out_width, int *out_height, int *out_stride,
                int *offset_x, int *offset_y)
{
  SfFilterParams med_p;
  SfFilterParams conv_p;
  SfFilterParams thr_p;
  SfFilterInParams hist_p;
  
  int med_width = 0, med_height = 0;
  int conv_width = 0, conv_height = 0;
  
  int inp_width = col_last - col_first;
  int inp_height = row_last - row_first;
  const uint8_t *inp_src = p->src + row_first * p->stride + col_first;

  assert (0 <= row_first && row_first < p->height);
  assert (0 <= col_first && col_first < p->width);

  *out_p = inp_src;
  *out_width = inp_width;
  *out_height = inp_height;
  *out_stride = p->stride;
  *offset_x = 0;
  *offset_y = 0;

  /* Apply the median filter if required */
  if (params->kmed_size > 0) {
    med_p.width = *out_width;
    med_p.height = *out_height;
    med_p.src = *out_p;
    med_p.src_stride = *out_stride;
    
    med_width = med_p.width - params->kmed_size + 1;
    med_height = med_p.height - params->kmed_size + 1;
    
    realloc_target_buffer (&proc->med_buf[region_i],
                           &proc->med_buf_size[region_i],
                           med_width * med_height);
    
    med_p.dest = proc->med_buf[region_i];
    med_p.dest_stride = med_width;
    
    sf_filter_median (&med_p,
                      params->kmed_size);
    
    *out_p = med_p.dest;
    *out_width = med_width;
    *out_height = med_height;
    *out_stride = med_p.dest_stride;
    *offset_x += (params->kmed_size - 1) / 2;
    *offset_y += (params->kmed_size - 1) / 2;
  }
  
  /* Apply the convolutional filter if required */
  if (params->kconv_size > 0 && params->kconv_sum != 0) {
    conv_p.width = *out_width;
    conv_p.height = *out_height;
    conv_p.src = *out_p;
    conv_p.src_stride = *out_stride;
    
    conv_width = conv_p.width - params->kconv_size + 1;
    conv_height = conv_p.height - params->kconv_size + 1;
    
    realloc_target_buffer (&proc->conv_buf[region_i],
                           &proc->conv_buf_size[region_i],
                           conv_width * conv_height);
    
    conv_p.dest = proc->conv_buf[region_i];
    conv_p.dest_stride = conv_width;
    
    sf_filter_conv (&conv_p,
                    params->kconv_data,
                    params->kconv_size,
                    params->kconv_sum);
    
    *out_p = conv_p.dest;
    *out_width = conv_width;
    *out_height = conv_height;
    *out_stride = conv_p.dest_stride;
    *offset_x += (params->kconv_size - 1) / 2;
    *offset_y += (params->kconv_size - 1) / 2;
  }
  
  if (compute_histogram) {
    hist_p.src = *out_p;
    hist_p.width = *out_width;
    hist_p.height = *out_height;
    hist_p.stride = *out_stride;

    sf_filter_compute_histogram (&hist_p,
                                 proc->histogram,
                                 ARRAY_SIZE (proc->histogram));
  }
  
  /* Do binary thresholding if the threshold was set */
  if (params->threshold > 0) {
    thr_p.width = *out_width;
    thr_p.height = *out_height;
    thr_p.src = *out_p;
    thr_p.src_stride = *out_stride;
    
    realloc_target_buffer (&proc->thr_buf[region_i],
                           &proc->thr_buf_size[region_i],
                           thr_p.width * thr_p.height);
    
    thr_p.dest = proc->thr_buf[region_i];
    thr_p.dest_stride = thr_p.width;
    
    sf_filter_threshold (&thr_p,
                         params->threshold,
                         0,
                         255);
    
    *out_p = thr_p.dest;
    /* width, height, and offsets do not change with thresholding */
    *out_width = thr_p.width;
    *out_height = thr_p.height;
    *out_stride = thr_p.dest_stride;
  }
}
