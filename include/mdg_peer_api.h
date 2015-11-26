#ifndef __MDG_PEER_API
#define __MDG_PEER_API
#include <stdint.h>

#ifndef MDG_MAX_PAIRING_COUNT
# define MDG_MAX_PAIRING_COUNT 32
#endif
#ifndef MDG_MAX_CONNECTION_COUNT
# define MDG_MAX_CONNECTION_COUNT 16
#endif

#define PAIRING_DURATION_LIMIT 120

#define MDG_PEER_ID_SIZE 32
#define MDG_PEER_PUBLIC_KEY_SIZE 32
#define MDG_LICENSEKEY_SIZE 128
#define MAX_ENDPOINT_BYTES 64
#define MAX_PROTOCOL_BYTES 40
#define MAX_TOKEN_BYTES 40
#define MAX_TICKET_BYTES 40
#define MAX_ENDPOINT_ROUTES 4
#define MAX_OTP_BYTES 32
/* Lenght of part of otp, that is never sent to server: */
#define LOCAL_PART_OTP_LEN 3
/* Rest of OTP is for server (when remote) and at least this many bytes: */
#define SERVER_PART_OTP_MIN_LEN 4

#define MDG_CONNECTION_STATUS_PEER_OFFLINE 0
#define MDG_CONNECTION_STATUS_PEER_LOCAL   1
#define MDG_CONNECTION_STATUS_PEER_GRID    2

/* Holds internal type definitions for portability: */
#include "mdg_internal_t.h"

_MDG_API_ int32_t mdg_init(uint32_t flags);
_MDG_API_ void mdg_run_main_loop();
#if MDG_USE_MDNS
_MDG_API_ int32_t mdg_set_mdns_service_type(char *name);
#endif

typedef void* mdg_peer_id_t;
typedef enum {
  mdg_pairing_mode_disabled = 0,
  mdg_pairing_mode_waiting_for_pairing_attempt = 1,
  mdg_pairing_mode_pairing_attempt_in_progress = 2,
} mdg_pairing_mode;

typedef struct {
  uint32_t unixtime_utc;    /* Zero when not known, otherwise "UTC seconds since epoch=1970". */
  uint8_t connect_switch;   /* 0/1 does client desire a mdg grid connection */
  uint8_t connected_to_mdg; /* 0/1 does mdg consider this device to be connected to mdg grid? */
  mdg_pairing_mode pairing_mode;     /* State of pairing state machine */
  uint8_t aggressive_ping;  /* 0/1 is aggressive ping enabled? */
  uint8_t remote_logging_enabled; /* Is remote logging enabled at the moment? */
} mdg_status_t;

_MDG_API_ int32_t mdg_status(mdg_status_t *status);
_MDG_API_ int32_t mdg_aggressive_ping(uint32_t duration);
_MDG_API_ int32_t mdg_make_private_key(void *private_key);
_MDG_API_ int32_t mdg_whoami(mdg_peer_id_t *device_id);
typedef struct {
  char *name;
  char *value;
} mdg_property_t;
_MDG_API_ int32_t mdg_connect(mdg_property_t *properties);
_MDG_API_ int32_t mdg_disconnect(void);

typedef struct {
  mdg_pairing_mode pairing_mode;
  uint32_t start_timestamp;
  uint32_t end_timestamp;
  char otp[MAX_OTP_BYTES];
  mdg_peer_id_t new_peer;
} mdg_pairing_mode_state_t;


typedef enum {
        mdg_pairing_starting = 0,
        mdg_pairing_otp_ready = 1,
        mdg_pairing_completed = 2,
        mdg_pairing_failed_generic = 3,
        mdg_pairing_failed_otp = 4
} mdg_pairing_status;

/* Callback invoked by MDG on state changes. Do not refer state after returning. */
extern void mdguser_pairing_state(mdg_pairing_mode_state_t* state, mdg_pairing_status status);
_MDG_API_ int32_t mdg_enable_pairing_mode(uint32_t duration);
_MDG_API_ void mdg_disable_pairing_mode(void);
_MDG_API_ int32_t mdg_revoke_pairing(mdg_peer_id_t device_id);
_MDG_API_ int32_t mdg_add_pairing(mdg_peer_id_t device_id);
_MDG_API_ int32_t mdg_revoke_all_pairings(void);
_MDG_API_ int32_t mdg_pair_remote(char *otp);
_MDG_API_ int32_t mdg_pair_local(char *otp, char *peer_ip, uint16_t port);
typedef enum {
        mdg_control_not_desired = 0,
        mdg_control_connecting = 1,
        mdg_control_connected = 2,
        mdg_control_failed = 3
} mdg_control_status_t;

/* Implement this callback: */
void mdguser_control_status(mdg_control_status_t state);
typedef enum {
        mdg_routing_disconnected = 0,
        mdg_routing_connected = 1,
        mdg_routing_failed = -1,
        mdg_routing_peer_not_available = -2,
        mdg_routing_peer_not_paired = -3
} mdg_routing_status_t;

/* Implement this callback: */
void mdguser_routing(uint32_t connection_id, mdg_routing_status_t state);
_MDG_API_ int32_t mdg_place_call_remote(mdg_peer_id_t device_id, char *protocol, uint32_t *connection_id, uint32_t timout);
_MDG_API_ int32_t mdg_place_call_local(mdg_peer_id_t device_id, char *protocol, char *peer_ip, uint16_t port, uint32_t *connection_id, uint32_t timeout);
typedef void (*mdg_receive_data)(const unsigned char *data, uint32_t count, uint32_t connection_id);
/* Arguments sent when invoking mdg_receive_data:
 * `data` = memory buffer holding data received. Only valid during invocation of call.
 * `count` = number of bytes in buffer
 * `connection_id` that identifies this connection. (Use in `mdg_send_to_peer` when sending data)
 */
_MDG_API_ int32_t mdg_receive_from_peer(uint32_t connection_id, mdg_receive_data cb);
_MDG_API_ int32_t mdg_send_to_peer(uint8_t *data, uint32_t count, uint32_t connection_id);
_MDG_API_ int32_t mdg_close_peer_connection(uint32_t connection_id);
extern int32_t mdguser_incoming_call(const char *protocol);
typedef int32_t (*mdg_peer_verifying_cb)(const char *protocol, const mdg_peer_id_t device_id);
_MDG_API_ void mdg_set_peer_verifying_cb(mdg_peer_verifying_cb cb);
_MDG_API_ int32_t mdg_get_connection_info(uint32_t connection_id,
                                          mdg_peer_id_t sender_device_id,
                                          char protocol[MAX_PROTOCOL_BYTES]);
_MDG_API_ int32_t mdg_enable_remote_logging(uint32_t duration);
_MDG_API_ int32_t mdg_set_debug_log_target(int32_t target);
#endif
