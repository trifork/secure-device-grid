
#ifndef __MDG_UTIL_H__
#define __MDG_UTIL_H__

#include <stdint.h>

/* Encode exactly `input_size` bytes of data at `input` into `dst`.
 * Creates zero-terminated string in dst.
 * Make sure dst is at least 2 * inputsize + 1 bytes large
 */
void hex_encode_bytes(const uint8_t *input, char *dst, int input_size);

/* Decode into exactly `output_size` bytes of data from `input` into `dst`.
 * Reads a zero-terminated string at input dst.
 * Make sure dst is at least 2 * inputsize + 1 bytes large. 
 * Anything but [0-9a-fA-F] is ignored in input.
 */
int hex_decode_bytes(char *input, uint8_t *dst, int output_size);

#ifdef DEBUG
extern void hexdump(const char *name, const void* d, uint16_t len);
#else
#define hexdump(X,Y,Z)
#endif

#endif
