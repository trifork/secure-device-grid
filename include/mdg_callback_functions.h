#include "mdg_internal_t.h"

typedef void(*mdg_abort_callback)();
typedef int(*incoming_call_from_peer_callback)(const char *protocol);
typedef int(*mdgstorage_add_pairing_callback)(uint8_t *peer_id);
typedef int(*mdgstorage_load_pairing_callback)(int pairing_index, uint8_t *peer_id);
typedef int(*mdgstorage_load_license_key_callback)(void *license_key);
typedef int(*mdgstorage_load_preset_pairing_token_callback)(char *token);
typedef int(*mdgstorage_load_private_key_callback)(void *private_key);
typedef int(*mdgstorage_remove_pairing_callback)(unsigned char *peer_id);
typedef int(*mdgstorage_load_random_base_callback)(uint8_t *random_base, uint32_t length);
typedef void(*mdguser_pairing_state_callback)(uint8_t pairing_mode, uint32_t start_timestamp, uint32_t end_timestamp, char *otp, uint8_t *new_peer, int32_t status);
typedef void(*mdguser_routing_callback)(uint32_t connection_id, int state);
typedef void(*mdguser_control_status_callback)(int32_t status);

_MDG_API_ int register_mdg_abort_callback(mdg_abort_callback callback);
_MDG_API_ int register_incoming_call_from_peer_callback(incoming_call_from_peer_callback callback);
_MDG_API_ int register_mdgstorage_add_pairing_callback(mdgstorage_add_pairing_callback callback);
_MDG_API_ int register_mdgstorage_load_pairing_callback(mdgstorage_load_pairing_callback callback);
_MDG_API_ int register_mdgstorage_load_license_key_callback(mdgstorage_load_license_key_callback callback);
_MDG_API_ int register_mdgstorage_load_preset_pairing_token_callback(mdgstorage_load_preset_pairing_token_callback callback);
_MDG_API_ int register_mdgstorage_load_private_key_callback(mdgstorage_load_private_key_callback callback);
_MDG_API_ int register_mdgstorage_remove_pairing_callback(mdgstorage_remove_pairing_callback callback);
_MDG_API_ int register_mdgstorage_load_random_base_callback(mdgstorage_load_random_base_callback callback);
_MDG_API_ int register_mdguser_pairing_state_callback(mdguser_pairing_state_callback callback);
_MDG_API_ int register_mdguser_routing_callback(mdguser_routing_callback callback);
_MDG_API_ int register_mdguser_control_status(mdguser_control_status_callback callback);

_MDG_API_ int mdg_connect_from_managed(uint8_t *properties, uint32_t count); //delegates to mdg_connect after malloc'ing space for properties
