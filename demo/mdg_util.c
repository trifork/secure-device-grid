
#ifdef DEBUG
#include <stdio.h>
#endif

#include "mdg_util.h"
#include "mdg_defines.h"

static char __hex[] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};


void hex_encode_bytes(const uint8_t *input, char *dst, int input_size)
{
  while (--input_size >= 0) {
    uint8_t b = *input++;
    *dst++ = __hex[(b >> 4) & 0x0f];
    *dst++ = __hex[b & 0x0f];
  }
  *dst = 0;
}

static int decode_hex_bits(uint8_t *b, char **input)
{
  char *p = *input, c = *p;
  while (c != 0) {
    if ('0' <= c && c <= '9') {
      *b = (c - '0');
      *input = p+1;
      return 0;
    }
    if ('a' <= c && c <= 'f') {
      *b = 10 + (c - 'a');
      *input = p+1;
      return 0;
    }
    if ('A' <= c && c <= 'F') {
      *b = 10 + (c - 'A');
      *input = p+1;
      return 0;
    }
    p++;
    c = *p;
  }
  *input = p;
  return 1;
}

int hex_decode_bytes(char *input, uint8_t *dst, int output_size)
{
  uint8_t b1, b2;
  while (--output_size >= 0) {
    if (decode_hex_bits(&b1, &input) || decode_hex_bits(&b2, &input)) {
      return 1;
    }
    *dst = (b1 << 4) | b2;
    dst++;
  }
  // Now check that there is no more input, apart from skipped chars.
  return !decode_hex_bits(&b1, &input);
}


#ifdef DEBUG
static char hexdump_buffer[200];
void hexdump(const char *name, const void* in, uint16_t len) /* For debug print only */
{
  const uint8_t *d = in;
  int line = 0, pos = 0;


  eprintf("== %s ==\n", name);
  while (line*16 + pos < len) {
    int line_head = (line*16);
    char *t = hexdump_buffer;
    t += sprintf(t, "%c%c%c%c: ", __hex[(line_head >> 12)&0xf], __hex[(line_head>>8)&0xf], __hex[(line_head>>4)&0xf], __hex[line_head&0xf]);
    for (pos = 0; pos < 16; pos++) {

      int p = line_head+pos;

      if (p < len) {
        uint8_t val = d[ line*16+pos ];
        t += sprintf(t, "%c%c ", __hex[val >> 4], __hex[val & 0x0f]);
      } else {
        t += sprintf(t, "   ");
      }
    }

    t += sprintf(t, "  ");
    for (pos = 0; pos < 16; pos++)
      {
        int p = line_head+pos;
        if (p < len) {
          char c = d[p];
          if (c < 127 && c >= 32) {
            *t = c;
          } else {
            *t = '.';
          }
          t++;
        }
      }
    *t = 0;

    line += 1;
    pos = 0;
    eprintf("%s\n", hexdump_buffer);
  }
#if !DEBUG_TO_UDP
  eprintf("\n");
#endif
}
#else
#if defined(MDG_CC3200) || defined(MDG_UCOS)
void eprintf(const char *x, ...)
{}
#endif
#endif //DEBUG
