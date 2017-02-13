// NO INCLUDE GUARDS HERE

#if !defined (_SF_IO_H_INSIDE_) && !defined (SFIO_COMPILATION)
#error "Only <sfio.h> can be included directly."
#endif

#ifndef SF_IO_API
#  ifdef _WIN32
#    ifdef SFIO_COMPILATION
#      define SF_IO_API __declspec (dllexport)
#    else
#      define SF_IO_API __declspec (dllimport)
#    endif
#  else
#    define SF_IO_API
#  endif
#endif
