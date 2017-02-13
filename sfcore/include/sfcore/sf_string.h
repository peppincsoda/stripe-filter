#ifndef _SF_STRING_H_
#define _SF_STRING_H_

#include <sfcore/sf_core_header.h>

#include <string.h>

#if defined(_WIN32)

#define sf_strcasecmp _strcmpi

#else

#include <strings.h>

#define sf_strcasecmp strcasecmp

#endif

#endif  //  _SF_STRING_H_
