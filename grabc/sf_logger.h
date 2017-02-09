#ifndef _SF_LOGGER_H_
#define _SF_LOGGER_H_

void
sf_log_info (const char *format, ...);

void
sf_log_warning (const char *format, ...);

void
sf_log_error (const char *format, ...);

/* This one appends the last system error to the parameters in the output. */
void
sf_log_last_sys_error (const char *format, ...);

#endif  //  _SF_LOGGER_H_
