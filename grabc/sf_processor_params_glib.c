#include "sf_processor_params_glib.h"

void
sf_processor_params_load_from_g_key_file (SfProcessorParams *p,
                                          GKeyFile *key_file,
                                          const gchar *group_name)
{
  /* Reset all parameters to default values. */
  sf_processor_params_destroy (p);
  sf_processor_params_init (p);

  sf_key_file_get_boolean (key_file, group_name, "draw-overlay", &p->draw_overlay);
  sf_key_file_get_double_in_range (key_file, group_name, "frame-width", 0.1, 1.0, &p->f_width);
  sf_key_file_get_double_in_range (key_file, group_name, "frame-height", 0.1, 1.0, &p->f_height);
  sf_key_file_get_double_in_range (key_file, group_name, "frame-border", 0.1, 1.0, &p->f_border);

  {
    int kmed_size;

    if (sf_key_file_get_integer (key_file, group_name, "median-kernel-size", &kmed_size)) {
      if (kmed_size != 0 && kmed_size != 3 && kmed_size != 5) {
        sf_log_warning ("While reading %s/%s: Value must be 0, 3 or 5", group_name, "median-kernel-size");
      } else {
        p->kmed_size = kmed_size;
      }
    }
  }

  {
    gint *kconv_data = NULL;
    gsize length;
    GError *err = NULL;

    kconv_data = g_key_file_get_integer_list (key_file, group_name, "conv-kernel", &length, &err);
    if (err != NULL) {
      sf_log_warning ("While reading %s/%s: %s", group_name, "conv-kernel", err->message);
      g_clear_error (&err);
      g_assert (kconv_data == NULL);
    } else if (length != 0 && length != 9 && length != 25) {
      sf_log_warning ("While reading %s/%s: Kernel size must be either 0, 3 or 5", group_name, "conv-kernel");
      g_clear_pointer (&kconv_data, g_free);
    } else {
      sf_move_from_gbuffer ((void **) &p->kconv_data, (const void **) &kconv_data, length * sizeof (gint));
      switch (length) {
        case 0: p->kconv_size = 0; break;
        case 9: p->kconv_size = 3; break;
        case 25: p->kconv_size = 5; break;
        default: g_assert (0);
      }

      if (p->kconv_size != 0) {
        int kconv_sum;
        if (sf_key_file_get_integer (key_file, group_name, "conv-kernel-sum", &kconv_sum)) {
          if (kconv_sum <= 0) {
            sf_log_warning ("While reading %s/%s: Value must be >0", group_name, "conv-kernel-sum");
          } else {
            p->kconv_sum = kconv_sum;
          }
        }
      }

    }
  }

  {
    int threshold;

    if (sf_key_file_get_integer (key_file, group_name, "threshold", &threshold)) {
      if (threshold < 0 || threshold > 255) {
        sf_log_warning ("While reading %s/%s: %s", group_name, "threshold", "Value must be in range 0-255");
      } else {
        p->threshold = threshold;
      }
    }
  }

  {
    int s_height;

    if (sf_key_file_get_integer (key_file, group_name, "stripe-height", &s_height)) {
      if (s_height < 1 || s_height > 11 || (s_height % 2) != 1) {
        sf_log_warning ("While reading %s/%s: Value must be odd and between 1-11", group_name, "stripe-height");
      } else {
        p->s_height = s_height;
      }
    }
  }

  {
    char *str = NULL;
    if (sf_key_file_get_string (key_file, group_name, "histogram-region", &str)) {
      SfProcessRegion region;
      if (!sf_process_region_from_str (str, &region)) {
        sf_log_warning ("While reading %s/%s: %s", group_name, "histogram-region", "Value must be one of 'none', 'left', 'right' or 'overlay'");
      } else {
        p->histogram_region = region;
      }
      sf_clear_pointer (&str, sf_free);
    }
  }

  sf_key_file_get_boolean (key_file, group_name, "compute-otsu", &p->compute_otsu);
  sf_key_file_get_boolean (key_file, group_name, "dark-object", &p->dark_object);
}
