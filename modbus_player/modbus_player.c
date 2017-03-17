#include <stdio.h>

#include <sfio.h>

#include <assert.h>

#define IS_WHITESPACE(c)  (((c) == ' ') || ((c) == 9) || ((c) == '\r') || ((c) == '\n'))

typedef struct _SfApp SfApp;

struct _SfApp
{
  SfSerialParams *sparams;
  SfSerial *serial;
};

static SfApp *app;

SfApp *
sf_app_new (void)
{
  SfApp *app = (SfApp *) sf_malloc (sizeof (SfApp));

  app->sparams = sf_serial_params_new ();
  app->serial = sf_serial_new ();

  return app;
}

void
sf_app_free (SfApp *app)
{
  sf_clear_pointer (&app->serial, sf_serial_free);
  sf_clear_pointer (&app->sparams, sf_serial_params_free);

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

  sf_serial_params_load_from_g_key_file (app->sparams, key_file, "Serial");

  ret = true;

error:
  g_clear_pointer (&key_file, g_key_file_free);

  return ret;
}

bool
sf_app_open_output (SfApp *app)
{
  if (!sf_serial_open_port (app->serial, app->sparams))
    return false;

  sf_log_info ("Output goes to serial line %s", app->sparams->device_file);

  return true;
}

void
sf_app_close_output (SfApp *app)
{
  sf_serial_close_port (app->serial);
}

/* Parses a hexadecimal number from *ppstr and stores it in result.
   Returns true if sucessful.
 */
static bool
parse_hex (const char **ppstr, int *result)
{
  int len = 0;

  *result = 0;
  while (**ppstr != 0 && !IS_WHITESPACE (**ppstr)) {
    if ('a' <= **ppstr && **ppstr <= 'f')
      *result = (*result << 4) | (**ppstr - 'a' + 10);
    else if ('A' <= **ppstr && **ppstr <= 'F')
      *result = (*result << 4) | (**ppstr - 'A' + 10);
    else if ('0' <= **ppstr && **ppstr <= '9')
      *result = (*result << 4) | (**ppstr - '0');
    else
      goto error;

    (*ppstr)++;

    /* Handle overflow */
    len++;
    if (len > sizeof (*result) * 2)
      goto error;
  }

  return true;

error:
  *result = 0;
  return false;
}

/* Parses a decimal number from *ppstr and stores it in result.
   Returns true if sucessful.
 */
static bool
parse_dec (const char **ppstr, int *result)
{
  const bool neg = (**ppstr == '-');

  if (neg)
    (*ppstr)++;

  *result = 0;
  while (**ppstr != 0 && !IS_WHITESPACE (**ppstr)) {
    if ('0' <= **ppstr && **ppstr <= '9') {
      const char digit = **ppstr - '0';

      /* Handle overflow */
      if ((!neg && *result > INT_MAX / 10) ||
           (neg && -(*result) < INT_MIN / 10))
        goto error;

      *result = *result * 10;

      /* Handle overflow */
      if ((!neg && *result > INT_MAX - digit) ||
           (neg && -(*result) < INT_MIN + digit))
        goto error;

      *result = *result + digit;

    } else {
      goto error;
    }

    (*ppstr)++;
  }

  *result = neg ? -(*result) : *result;
  return true;

error:
  *result = 0;
  return false;
}

/* Parses a number from *ppstr and stores it in result.
   The number must be between min_value and max_value (inclusive).
   Returns true if sucessful.
 */
static bool
parse_int (const char **ppstr, int *result, int min_value, int max_value)
{
  /* Skip spaces */
  while (IS_WHITESPACE (**ppstr))
    (*ppstr)++;

  if (**ppstr == '0' && ((*ppstr)[1] == 'x' || (*ppstr)[1] == 'X')) {
    (*ppstr) += 2;
    if (!parse_hex (ppstr, result))
      return false;

  } else {
    if (!parse_dec (ppstr, result))
      return false;
  }

  if (*result < min_value || *result > max_value) {
    *result = 0;
    return false;
  }

  return true;
}

static bool
parse_int32 (const char **ppstr, int *result)
{
  return parse_int (ppstr, result, INT_MIN, INT_MAX);
}

/* Parser testing function.
   This should be moved to a separate tests file.
 */
static void
test_parse_int ()
{
  const char *min = "-2147483648";
  const char *under_min = "-2147483649";
  const char *max = "2147483647";
  const char *over_max = "2147483648";
  const char *max_hex = "0xffffffff";
  const char *over_max_hex = "0x100000000";

  int r;
  assert ( parse_int32 (&max, &r) && r == INT_MAX);
  assert (!parse_int32 (&over_max, &r) && r == 0);

  assert ( parse_int32 (&min, &r) && r == INT_MIN);
  assert (!parse_int32 (&under_min, &r) && r == 0);

  assert ( parse_int32 (&max_hex, &r) && r == 0xffffffff);
  assert (!parse_int32 (&over_max_hex, &r) && r == 0);
}

/* Parse all parameters of Modbus function 6 (preset single register) and send the assembled message.
   Parameter p is the current position in line_buffer.
 */
static bool
sf_app_parse_preset_single_register (SfApp *app, const char *line_buffer, const char **p, int lineno)
{
  int slave_address;
  int data_address;
  int value;
  uint8_t buffer[16];
  size_t bytes_written;

  if (!parse_int (p, &slave_address, 0, 247)) {
    sf_log_error ("at line %d column %d: Slave address expected in range 0-%d", lineno, (int)(*p - line_buffer) + 1, 247);
    goto error;
  }
  if (!parse_int (p, &data_address, 0, UINT16_MAX)) {
    sf_log_error ("at line %d column %d: Data address expected in range 0-%d", lineno, (int)(*p - line_buffer) + 1, UINT16_MAX);
    goto error;
  }
  if (!parse_int (p, &value, 0, UINT16_MAX)) {
    sf_log_error ("at line %d column %d: Value expected in range 0-%d", lineno, (int)(*p - line_buffer) + 1, UINT16_MAX);
    goto error;
  }

  bytes_written = sf_modbus_preset_single_register (buffer,
                                                    sizeof (buffer),
                                                    (uint8_t) slave_address,
                                                    (uint16_t) data_address,
                                                    (uint16_t) value);

  assert (bytes_written < sizeof (buffer));

  if (!sf_serial_write (app->serial, buffer, bytes_written))
    goto error;

  return true;

error:
  return false;
}

/* Parse all parameters of Modbus function 16 (preset multiple register) and send the assembled message.
   Parameter p is the current position in line_buffer.
 */
static bool
sf_app_parse_preset_multiple_registers (SfApp *app, const char *line_buffer, const char **p, int lineno)
{
  int slave_address;
  int data_address;
  int num_registers;
  uint16_t *values = NULL;
  uint8_t *buffer = NULL;
  size_t bytes_written;
  int i;
  bool ret = false;

  if (!parse_int (p, &slave_address, 0, 247)) {
    sf_log_error ("at line %d column %d: Slave address expected in range 0-%d", lineno, (int)(*p - line_buffer) + 1, 247);
    goto error;
  }
  if (!parse_int (p, &data_address, 0, UINT16_MAX)) {
    sf_log_error ("at line %d column %d: Data address expected in range 0-%d", lineno, (int)(*p - line_buffer) + 1, UINT16_MAX);
    goto error;
  }
  if (!parse_int (p, &num_registers, 0, UINT16_MAX)) {
    sf_log_error ("at line %d column %d: Number of registers expected in range 0-%d", lineno, (int)(*p - line_buffer) + 1, UINT16_MAX);
    goto error;
  }
  values = (uint16_t *) sf_malloc (num_registers * sizeof (uint16_t));
  for (i = 0; i < num_registers; i++) {
    int value;
    if (!parse_int (p, &value, 0, UINT16_MAX)) {
      sf_log_error ("at line %d column %d: Value expected in range 0-%d", lineno, (int)(*p - line_buffer) + 1, UINT16_MAX);
      goto error;
    }
    values[i] = (uint16_t) value;
  }

  /* The first call returns the buffer size required to store the message. */
  bytes_written = sf_modbus_preset_multiple_registers (NULL,
                                                       0,
                                                       (uint8_t) slave_address,
                                                       (uint16_t) data_address,
                                                       (uint16_t) num_registers,
                                                       values);

  buffer = (uint8_t *) sf_malloc (bytes_written);

  bytes_written = sf_modbus_preset_multiple_registers (buffer,
                                                       bytes_written,
                                                       (uint8_t) slave_address,
                                                       (uint16_t) data_address,
                                                       (uint16_t) num_registers,
                                                       values);

  if (!sf_serial_write (app->serial, buffer, bytes_written))
    goto error;

  ret =  true;

error:
  sf_clear_pointer (&buffer, sf_free);
  sf_clear_pointer (&values, sf_free);

  return ret;
}

/* Read the specified file and execute the requested Modbus function at each line.
 */
static bool
sf_app_parse_file (FILE *file)
{
  char line_buffer[1024];
  int lineno = 0;

  while (fgets (line_buffer, ARRAY_SIZE (line_buffer), file) != NULL) {
    const char *p = line_buffer;
    int delay_ms;
    int function_code;

    lineno++;

    if (strlen (line_buffer) == ARRAY_SIZE (line_buffer) - 1) {
      sf_log_error ("at line %d: Line too long", lineno);
      goto error;
    }

    /* Skip comment lines */
    if (line_buffer[0] == '#')
      continue;

    if (!parse_int32 (&p, &delay_ms)) {
      sf_log_error ("at line %d column %d: Delay in milliseconds expected", lineno, (int)(p - line_buffer) + 1);
      goto error;
    }

    sf_sleep (delay_ms);

    if (!parse_int32 (&p, &function_code)) {
      sf_log_error ("at line %d column %d: Modbus function code expected", lineno, (int)(p - line_buffer) + 1);
      goto error;
    }
    if (function_code == 6) {
      if (!sf_app_parse_preset_single_register (app, line_buffer, &p, lineno))
        goto error;
    } else if (function_code == 16) {
      if (!sf_app_parse_preset_multiple_registers (app, line_buffer, &p, lineno))
        goto error;
    } else {
      sf_log_error ("at line %d column %d: Allowed Modbus function codes are 6 and 16", lineno, (int)(p - line_buffer) + 1);
      goto error;
    }
  }

  return true;

error:
  return false;
}

int main (int argc, char *argv[])
{
  int ret = 1;
  const char *settings_path;

  test_parse_int ();

  if (argc < 2)
    settings_path = "settings.ini";
  else
    settings_path = argv[1];

#ifdef _WIN32
  /* Report memory leaks on Windows. */
  _CrtSetDbgFlag (_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

  app = sf_app_new ();

  if (!sf_app_load_settings_from_file (app, settings_path)) {
    if (argc < 2)
      sf_log_info ("Usage: %s <settings-file>\n", argv[0]);

    goto error;
  }

  if (!sf_app_open_output (app))
    goto error;

  if (!sf_app_parse_file (stdin))
    goto error;

  /* no error */
  ret = 0;

error:
  sf_app_close_output (app);

  sf_clear_pointer (&app, sf_app_free);

  sf_log_info ("Exiting...\n");

  return ret;
}
