// NO INCLUDE GUARDS HERE

#if !defined (_SF_CORE_H_INSIDE_) && !defined (SFCORE_EXPORTS)
#error "Only <sfcore.h> can be included directly."
#endif

#ifndef SF_CORE_API
#  ifdef SFCORE_EXPORTS
#    define SF_CORE_API __declspec (dllexport)
#  else
#    define SF_CORE_API __declspec (dllimport)
#  endif
#endif
