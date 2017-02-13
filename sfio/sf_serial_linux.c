#include <sfio/sf_serial.h>

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

/* Source: https://www.cmrr.umn.edu/~strupp/serial.html */

struct _SfSerial
{
  int fd;
};

SfSerial *
sf_serial_new (void)
{
  SfSerial *p;

  p = (SfSerial *) sf_malloc (sizeof (SfSerial));
  p->fd = -1;

  return p;
}

void
sf_serial_free (SfSerial *p)
{
  if (p->fd != -1)
    sf_serial_close_port (p);

  sf_free (p);
}

static bool
sf_serial_baud_rate_to_speed (int baud_rate, speed_t *speed)
{
  static int baud_rates[] = { 50, 75, 110, 134, 150, 200, 300,
    600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
  static speed_t speeds[] = { B50, B75, B110, B134, B150, B200, B300,
    B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200 };

  int i = sf_binary_search (baud_rates, ARRAY_SIZE (baud_rates), baud_rate);
  if (i == -1)
    return false;
  
  *speed = speeds[i];
  return true;
}

bool
sf_serial_open_port (SfSerial *p, SfSerialParams *params)
{
  struct termios options;

  assert (p->fd == -1);

  p->fd = open (params->device_file, O_RDWR | O_NOCTTY | O_NDELAY);
  if (p->fd == -1) {
    sf_log_last_sys_error ("Cannot open serial port");
    goto error;
  }

  /* Set blocking behaviour for read operations. */
  if (fcntl (p->fd, F_SETFL, 0) == -1) {
    sf_log_last_sys_error ("fcntl returned with error");
    goto error;
  }

  if (tcgetattr (p->fd, &options) == -1) {
    sf_log_last_sys_error ("tcgetattr returned with error");
    goto error;
  }

  /* Set baud rate. */
  {
    speed_t speed;

    if (!sf_serial_baud_rate_to_speed (params->baud_rate, &speed)) {
      sf_log_warning ("Unsupported baud rate in serial settings; 19200 baud is used");
      speed = B19200;
    }

    cfsetispeed (&options, speed);
    cfsetospeed (&options, speed);
  }

  /* Enable the receiver and set local mode. */
  options.c_cflag |= (CLOCAL | CREAD);

  /* Set parity. */
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~PARODD;
  switch (params->parity) {
  case SfSerialParity_None: break;
  case SfSerialParity_Even: options.c_cflag |= PARENB; break;
  case SfSerialParity_Odd:  options.c_cflag |= PARODD; break;
  default:
    assert (0);
  }

  /* Set the number of data bits. */
  options.c_cflag &= ~CSIZE;
  switch (params->data_bits) {
  case 7: options.c_cflag |= CS7; break;
  case 8: options.c_cflag |= CS8; break;
  default:
    assert (0);
  }

  /* Set the number of stop bits. */
  options.c_cflag &= ~CSTOPB;
  switch (params->stop_bits) {
  case 1: break;
  case 2: options.c_cflag |= CSTOPB; break;
  default:
    assert (0);
  }

  if (tcsetattr (p->fd, TCSANOW, &options) == -1) {
    sf_log_last_sys_error ("tcsetattr returned with error");
    goto error;
  }

  return true;

error:
  if (p->fd != -1) {
    close (p->fd);
    p->fd = -1;
  }

  return false;
}

void
sf_serial_close_port (SfSerial *p)
{
  assert (p->fd != -1);

  close (p->fd);
  p->fd = -1;
}

bool
sf_serial_write (SfSerial *p,
                 const void *buffer,
                 int buffer_size)
{
  assert (p->fd != -1);
  
  if (write (p->fd, buffer, buffer_size) < 0) {
    sf_log_last_sys_error ("Cannot write serial data");
    return false;
  }

  return true;
}
