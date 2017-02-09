#include "sf_memory.h"

#include "sf_logger.h"
#include "sf_camera.h"
#include "sf_processor.h"
#include "sf_serial.h"
#include "sf_file_monitor.h"
#include "sf_init_status.h"
#include "sf_thread.h"
#include "sf_modbus.h"
#include "sf_time.h"

#include "sf_processor_params_glib.h"
#include "sf_serial_params_glib.h"
#include "sf_app_params_glib.h"

#include <glib.h>

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

/* This is the maximum period of sending output messages in milliseconds. */
#define MAX_OUTPUT_PERIOD_MS 100

typedef struct _SfApp SfApp;

struct _SfApp
{
  SfAppParams *app_params;
  SfProcessorParams *pparams;
  SfProcessor *processor;
  SfSerialParams *sparams;
  SfSerial *serial;

  bool exit_requested;
  SfFileMonitor *settings_monitor;

  SfInitStatus input_init_status;
  SfInitStatus output_init_status;

  SfCamera *camera;

  uint64_t output_last_write_time;
  uint64_t main_loop_last_start_time;
};

typedef enum _SfAppOutputStatus SfAppOutputStatus;

enum _SfAppOutputStatus
{
  SfAppOutputStatus_OK = 1,
  SfAppOutputStatus_ProcessingFailed,
  SfAppOutputStatus_InputFailed,
};

typedef struct _SfAppOutputData SfAppOutputData;

struct _SfAppOutputData
{
  int thickness;
  SfAppOutputStatus status;
};

static SfApp *app;

bool
sf_app_open_input (void *context);

bool
sf_app_open_output (void *context);

void
sf_app_camera_frame_fn (void *context, SfCameraGrabResult *result);

const char *
sf_app_output_status_to_string (SfAppOutputStatus status)
{
  switch (status) {
  case SfAppOutputStatus_OK:                return "OK";
  case SfAppOutputStatus_ProcessingFailed:  return "ProcessingFailed";
  case SfAppOutputStatus_InputFailed:       return "InputFailed";
  }

  assert (0);
  return NULL;
}

SfApp *
sf_app_new (void)
{
  SfApp *app = (SfApp *) sf_malloc (sizeof (SfApp));

  app->app_params = sf_app_params_new ();
  app->pparams = sf_processor_params_new ();
  app->processor = sf_processor_new ();
  app->sparams = sf_serial_params_new ();
  app->serial = sf_serial_new ();

  app->exit_requested = false;
  app->settings_monitor = sf_file_monitor_new ();

  sf_init_status_init (&app->input_init_status, 3, 1000, 30000, sf_app_open_input, app);
  sf_init_status_init (&app->output_init_status, 3, 1000, 30000, sf_app_open_output, app);

  app->camera = NULL;

  app->output_last_write_time = 0;
  app->main_loop_last_start_time = 0;

  return app;
}

void
sf_app_free (SfApp *app)
{
  sf_clear_pointer (&app->camera, sf_camera_free);

  sf_init_status_destroy (&app->output_init_status);
  sf_init_status_destroy (&app->input_init_status);

  sf_clear_pointer (&app->settings_monitor, sf_file_monitor_free);

  sf_clear_pointer (&app->serial, sf_serial_free);
  sf_clear_pointer (&app->sparams, sf_serial_params_free);
  sf_clear_pointer (&app->processor, sf_processor_free);
  sf_clear_pointer (&app->pparams, sf_processor_params_free);
  sf_clear_pointer (&app->app_params, sf_app_params_free);

  sf_free (app);
}

bool
sf_app_load_settings_from_file (SfApp *app, const char *file_name)
{
  GKeyFile *key_file = g_key_file_new ();
  GError *err = NULL;
  bool ret = false;

  /* Load all parameters from key_file. */
  if (!g_key_file_load_from_file (key_file, file_name, G_KEY_FILE_NONE, &err)) {
    sf_log_error ("While loading settings file: %s", err->message);
    g_clear_error (&err);
    goto error;
  }

  sf_app_params_load_from_g_key_file (app->app_params, key_file, "Application");
  sf_processor_params_load_from_g_key_file (app->pparams, key_file, "Filter");
  sf_serial_params_load_from_g_key_file (app->sparams, key_file, "Serial");

  ret = true;

error:
  g_clear_pointer (&key_file, g_key_file_free);

  return ret;
}

bool
sf_app_open_input (void *context)
{
  SfApp *app = (SfApp *) context;

  assert (app->camera == NULL);

  app->camera = sf_camera_new ();
  if (app->camera == NULL)
    return false;

  return true;
}

bool
sf_app_open_output (void *context)
{
  SfApp *app = (SfApp *) context;

  switch (app->app_params->output) {
  case SfAppOutput_None:
    sf_log_info ("Output is turned off");
    break;

  case SfAppOutput_StdOut:
    sf_log_info ("Output goes to STDOUT");
    break;

  case SfAppOutput_Serial:
    if (!sf_serial_open_port (app->serial, app->sparams))
      return false;

    sf_log_info ("Output goes to serial line %s", app->sparams->device_file);
    break;
  }

  return true;
}

void
sf_app_close_input (SfApp *app)
{
  sf_clear_pointer (&app->camera, sf_camera_free);
  sf_init_status_reset (&app->input_init_status);
  sf_log_info ("Resetting input");
}

void
sf_app_close_output (SfApp *app)
{
  if (app->app_params->output == SfAppOutput_Serial) {
    sf_serial_close_port (app->serial);
  }
  sf_init_status_reset (&app->output_init_status);
  sf_log_info ("Resetting output");
}

bool
sf_app_write_output (SfApp *app, SfAppOutputData *output_data)
{
  if (app->app_params->output == SfAppOutput_Serial) {
    uint16_t values[] = {(uint16_t) output_data->thickness,
                         (uint16_t) output_data->status};
    uint8_t buffer[9 + sizeof (values)];

    /* Make sure we didn't lose data because of the conversion to 16-bit */
    assert ((int) values[0] == output_data->thickness);
    assert ((SfAppOutputStatus) values[1] == output_data->status);

    sf_modbus_preset_multiple_registers (buffer,
                                         sizeof (buffer),
                                         app->app_params->modbus_slave_address,
                                         app->app_params->modbus_register_address,
                                         ARRAY_SIZE (values),
                                         values);

    if (!sf_serial_write (app->serial, buffer, sizeof (buffer)))
      return false;

  } else if (app->app_params->output == SfAppOutput_StdOut) {
    sf_log_info ("Output value: %d; status: %s",
      output_data->thickness,
      sf_app_output_status_to_string (output_data->status));
  }

  return true;
}

bool
sf_app_try_read_input (SfApp *app)
{
  /* Check if the input is initialized. */
  if (!sf_init_status_try (&app->input_init_status))
    return false;

  if (sf_camera_grab_image (app->camera, sf_app_camera_frame_fn, app) != SfCameraGrabStatus_Grabbed) {
    sf_app_close_input (app);
    return false;
  }

  return true;
}

bool
sf_app_try_write_output (SfApp *app, SfAppOutputData *output_data)
{
  /* Output frequency is maximized here to prevent flooding the serial
     line / stdout if the main loop iteration frequency is increased. */
  uint64_t now = sf_time_get_tick_count ();

  if (now - app->output_last_write_time < MAX_OUTPUT_PERIOD_MS)
    return true;

  app->output_last_write_time = now;

  /* Check if the output is initialized. */
  if (!sf_init_status_try (&app->output_init_status))
    return false;

  if (!sf_app_write_output (app, output_data)) {
    sf_app_close_output (app);
    return false;
  }
  
  return true;
}

void
sf_app_camera_frame_fn (void *context, SfCameraGrabResult *result)
{
  SfApp *app = (SfApp *) context;
  SfFilterParams fparams;
  SfAppOutputData output_data;

  /* Copy camera output parameters to the filter. */
  fparams.width       = result->width;
  fparams.height      = result->height;
  fparams.src         = result->buffer;
  fparams.src_stride  = result->stride;
  fparams.dest        = result->buffer;
  fparams.dest_stride = result->stride;

  if (sf_processor_process_image (app->processor,
                                  &fparams,
                                  app->pparams,
                                  &output_data.thickness))
    output_data.status = SfAppOutputStatus_OK;
  else
    output_data.status = SfAppOutputStatus_ProcessingFailed;

  /* Write output to the serial line or to the console */
  sf_app_try_write_output (app, &output_data);
}

void
sf_app_on_main_loop_start (SfApp *app)
{
  app->main_loop_last_start_time = sf_time_get_tick_count ();
}

void
sf_app_on_main_loop_end (SfApp *app)
{
  /* Sleep for 50ms if the main loop took <50ms to avoid busy looping */
  uint64_t now = sf_time_get_tick_count ();
  int time_diff = (int) (now - app->main_loop_last_start_time);
  if (time_diff < 50)
    sf_sleep (50 - time_diff);
}

static void
sig_handler (int signo)
{
  if (signo == SIGINT ||
      signo == SIGTERM) {
    app->exit_requested = true;
  }
}

static void
on_settings_changed (SfFileMonitor *monitor,
                     const char *path,
                     void *context)
{
  //SfApp *app = (SfApp *) context;

  sf_log_info ("Settings were changed; reloading");
}

int main (int argc, char *argv[])
{
  int ret = 1;
  const char *settings_path;

  if (argc < 2) {
    fprintf (stderr, "Usage: %s <settings-file>\n", argv[0]);
    goto error;
  }
  settings_path = argv[1];

  sf_log_info ("Service started; Settings path is: %s", settings_path);

#ifdef _WIN32
  /* Report memory leaks on Windows. */
  _CrtSetDbgFlag (_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

  /* Install signal handler for Ctrl+C. */
  if (signal (SIGINT, sig_handler) == SIG_ERR ||
      signal (SIGTERM, sig_handler) == SIG_ERR) {
    sf_log_error ("Cannot install SIGINT handler");
    goto error;
  }

  app = sf_app_new ();

  if (!sf_file_monitor_add_watch (app->settings_monitor,
                                  settings_path,
                                  on_settings_changed,
                                  app)) {
    sf_log_error ("Cannot install file monitor");
    goto error;
  }

  if (!sf_app_load_settings_from_file (app, settings_path))
    goto error;

  while (!app->exit_requested) {
    sf_app_on_main_loop_start (app);

    /* Reload settings if the settings file has been changed. */
    if (sf_file_monitor_poll (app->settings_monitor)) {
      sf_app_close_input (app);
      sf_app_close_output (app);

      if (!sf_app_load_settings_from_file (app, settings_path))
        goto error;
    }

    if (!sf_app_try_read_input (app)) {
      /* Send status update to the output */
      SfAppOutputData output_data;
      output_data.thickness = 0;
      output_data.status = SfAppOutputStatus_InputFailed;
      sf_app_try_write_output (app, &output_data);
    }

    sf_app_on_main_loop_end (app);
  }

  /* no error */
  ret = 0;

error:
  sf_clear_pointer (&app, sf_app_free);

  sf_log_info ("Exiting...\n");

  return ret;
}
