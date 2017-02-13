#include <sfcore.h>
#include <sfio/sf_serial.h>

#include <assert.h>

#include <Windows.h>

struct _SfSerial
{
  HANDLE hFile;
};

SF_IO_API
SfSerial *
sf_serial_new (void)
{
  SfSerial *p;

  p = (SfSerial *) sf_malloc (sizeof (SfSerial));
  p->hFile = INVALID_HANDLE_VALUE;

  return p;
}

SF_IO_API
void
sf_serial_free (SfSerial *p)
{
  if (p->hFile != INVALID_HANDLE_VALUE)
    sf_serial_close_port (p);

  sf_free (p);
}

SF_IO_API
bool
sf_serial_open_port (SfSerial *p, SfSerialParams *params)
{
  DCB dcb;
  COMMPROP commprop;

  assert (p->hFile == INVALID_HANDLE_VALUE);

  p->hFile = CreateFile (params->device_file,
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         0,
                         OPEN_EXISTING,
                         0,
                         0);
  if (p->hFile == INVALID_HANDLE_VALUE) {
    sf_log_last_sys_error ("Cannot open serial port");
    goto error;
  }

  ZeroMemory (&dcb, sizeof(dcb));
  if (!GetCommState (p->hFile, &dcb)) {
    // get current DCB
    sf_log_last_sys_error ("Cannot retrieve comm state");
    goto error;
  }
  
  // Update DCB.
  dcb.BaudRate = params->baud_rate;
  
  switch (params->parity) {
  case SfSerialParity_None: dcb.Parity = NOPARITY; break;
  case SfSerialParity_Even: dcb.Parity = EVENPARITY; break;
  case SfSerialParity_Odd:  dcb.Parity = ODDPARITY; break;
  default:
    assert (0);
  }

  switch (params->data_bits)
  {
  case 7: dcb.ByteSize = 7; break;
  case 8: dcb.ByteSize = 8; break;
  default:
    assert (0);
  }

  switch (params->stop_bits) {
  case 1: dcb.StopBits = ONESTOPBIT; break;
  case 2: dcb.StopBits = TWOSTOPBITS; break;
  default:
    assert (0);
  }
  
  dcb.fBinary = TRUE; // Use binary mode
  // Do not block if the other end is not ready to receive our data
  dcb.fOutxCtsFlow = FALSE; 
  dcb.fOutxDsrFlow = FALSE;
  dcb.fDtrControl = DTR_CONTROL_ENABLE; // We are always ready to receive
  dcb.fRtsControl = RTS_CONTROL_ENABLE; // Ask the other end to send data if available

  // Set new state.
  if (!SetCommState (p->hFile, &dcb)) {
    // Error in SetCommState. Possibly a problem with the communications 
    // port handle or a problem with the DCB structure itself.
    sf_log_last_sys_error ("Cannot set new comm state");
    goto error;
  }

  ZeroMemory (&commprop, sizeof(commprop));
  if (!GetCommProperties (p->hFile, &commprop)) {
    sf_log_last_sys_error ("Cannot get serial line properties");
    goto error;
  }

  sf_log_info ("Serial output buffer size: %d byte(s)", commprop.dwMaxTxQueue);

  return true;

error:
  if (p->hFile != INVALID_HANDLE_VALUE) {
    CloseHandle (p->hFile);
    p->hFile = INVALID_HANDLE_VALUE;
  }

  return false;
}

SF_IO_API
void
sf_serial_close_port (SfSerial *p)
{
  if (p->hFile != INVALID_HANDLE_VALUE) {
    CloseHandle (p->hFile);
    p->hFile = INVALID_HANDLE_VALUE;
  }
}

SF_IO_API
bool
sf_serial_write (SfSerial *p,
                 const void *buffer,
                 int buffer_size)
{
  DWORD bytes_written;

  assert (p->hFile != INVALID_HANDLE_VALUE);

  if (!WriteFile (p->hFile, buffer, buffer_size, &bytes_written, NULL)) {
    sf_log_last_sys_error ("Cannot write serial port");
    return false;
  }

  return true;
}
