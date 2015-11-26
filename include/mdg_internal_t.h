#ifndef MDG_INTERNAL_T
#define MDG_INTERNAL_T

#ifdef MDG_WINDOWS
#ifdef EXPORT
#define _MDG_API_  __declspec(dllexport) extern
#else
#define _MDG_API_ __declspec(dllimport)
#endif
#elif __GNUC__ >= 4
#define _MDG_API_ extern __attribute__ ((visibility ("default")))
#else
#define _MDG_API_ extern
#endif

#if defined(MDG_WINDOWS) || defined(MDG_CC3200) || defined(MDG_UCOS)
#define noreturn
#else
#include <stdnoreturn.h>
#endif

#endif /* MDG_INTERNAL_T */
