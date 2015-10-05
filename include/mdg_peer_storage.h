#ifndef __MDG_PEER_STORAGE
#define __MDG_PEER_STORAGE
#include "mdg_peer_api.h"

#define MDG_PRIVATE_KEY_DATA_SIZE 32
#define MDG_PRESET_PAIR_ID_SIZE 32
#define MDG_MIN_PRESET_PAIR_ID_DIGITS 5

#define MDGSTORAGE_OK 0
extern int mdgstorage_add_pairing(uint8_t *device_id);
extern int mdgstorage_remove_pairing(uint8_t *peer_id);
extern int mdgstorage_load_pairing(int pairings_index, uint8_t *peer_id);
extern int mdgstorage_load_private_key(void *private_key);
extern int mdgstorage_load_license_key(void *license_key);
extern int mdgstorage_load_random_base(uint8_t *random_base, uint32_t length);
extern int mdgstorage_load_preset_pairing_token(char *token);
extern noreturn void mdg_abort();
#endif
