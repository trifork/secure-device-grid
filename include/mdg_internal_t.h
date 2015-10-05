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


#if MDG_UNIX
#include <netdb.h> //hostent
typedef struct hostent mdg_ip_addr_t;
#elif MDG_WINDOWS
typedef struct hostent mdg_ip_addr_t;
#elif MDG_CC3200 || MDG_WICED || MDG_UCOS
struct hostent {
	char  *h_name;      /* Official name of the host. */
	char **h_aliases;   /* A pointer to an array of pointers to alternative host names,
						terminated by a null pointer. */
	int    h_addrtype;  /* Address type. */
	int    h_length;    /* The length, in bytes, of the address. */
	char **h_addr_list; /* A pointer to an array of pointers to network addresses (in
				network byte order) for the host, terminated by a null pointer. */
#define h_addr h_addr_list[0] /* for backward compatibility */
};

typedef struct hostent mdg_ip_addr_t;
#endif

#if MDG_WICED
#if defined(NETWORK_NetX)
#include "nx_api.h"
#include "netx_applications/dns/nx_dns.h"
extern struct hostent* netx_gethostbyname(NX_DNS *dns_ptr, const CHAR *name);
#elif defined(NETWORK_NetX_Duo)
#include "nx_api.h"
#include "netx_applications/dns/nxd_dns.h"
extern struct hostent* netx_gethostbyname(NX_DNS *dns_ptr, const CHAR *name);
#endif

#endif /* MDG_WICED */

#endif /* MDG_INTERNAL_T */
