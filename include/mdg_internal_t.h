#ifndef MDG_INTERNAL_T
#define MDG_INTERNAL_T
#if defined(WINAPI_FAMILY) || defined(MDG_WINDOWS)
#ifdef EXPORT
#ifdef __cplusplus
#define _MDG_API_  __declspec(dllexport) extern "C"
#else
#define _MDG_API_  __declspec(dllexport) extern
#endif
#else
#ifdef __cplusplus
#define _MDG_API_  __declspec(dllimport) extern "C"
#else
#define _MDG_API_  __declspec(dllimport) extern
#endif
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
