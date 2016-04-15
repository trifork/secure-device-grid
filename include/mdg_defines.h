#ifndef _MDG_DEFINES_H
#define _MDG_DEFINES_H

#include <stddef.h>
#include <stdint.h>

#if !defined(checkreturn)
#if !defined(__GNUC__) || ( __GNUC__ < 3) || (__GNUC__ == 3 && __GNUC_MINOR__ < 4)
    #define checkreturn
#else
    #define checkreturn __attribute__((warn_unused_result))
#endif
#endif

#if defined(DEBUG)
// Include support for remote debug:
#define DEBUG_TO_UDP 1
#if defined(DEBUG_TO_UDP)
#define eprintf mdg_eprintf_udp
extern void mdg_eprintf_udp(const char *fmt, ...);
extern int mdg_debug_log_target;
#else /* DEBUG_TO_UDP */
#if defined(MDG_CC3200) || defined(MDG_UCOS)
/* Prints to uart: */
#define eprintf Report
#elif __STDC_VERSION__ >= 199901L || MDG_WINDOWS
/* C99 version: */
#include <stdio.h>
#define eprintf(...) fprintf (stderr, __VA_ARGS__)
#else
/* GCC version: */
#include <stdio.h>
#define eprintf(format, args...) fprintf (stderr, format, ##args)
#endif
#endif /* DEBUG_TO_UDP */

#else /* No DEBUG: */

#if defined(MDG_CC3200) || defined(MDG_UCOS)
void eprintf(const char *x, ...); //empty implementation in mdg_util.c, as compiler does not support a variable number of arguments
#elif __STDC_VERSION__ >= 199901L || MDG_WINDOWS
/* C99 version: */
#define eprintf(x,...)
#else
/* GCC version: */
#define eprintf(format, args...)
#endif

#endif /* DEBUG */

#endif /* _MDG_DEFINES_H */
