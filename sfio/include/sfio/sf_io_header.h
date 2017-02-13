// NO INCLUDE GUARDS HERE

#if !defined (_SF_IO_H_INSIDE_) && !defined (SFIO_EXPORTS)
#error "Only <sfio.h> can be included directly."
#endif

#ifndef SF_IO_API
#ifdef SFIO_EXPORTS
#define SF_IO_API __declspec (dllexport)
#else
#define SF_IO_API __declspec (dllimport)
#endif
#endif
