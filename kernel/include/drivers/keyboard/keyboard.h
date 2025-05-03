#pragma once

#include <drivers/keyboard/scancode.h>
#include <util/util.h>
#include <stdint.h>

#ifndef USE_KEY_RAW_SCANCODE
#define USE_KEY_RAW_SCANCODE 1
#endif

typedef enum {
  KEY_FLAG_NONE  = 0,
  KEY_FLAG_SHIFT = (1 << 0),
  KEY_FLAG_ALT   = (1 << 1),
  KEY_FLAG_CTRL  = (1 << 2),
  KEY_FLAG_CMD   = (1 << 3),
  KEY_FLAG_FN    = (1 << 4),
} key_flags_t;

typedef struct {
  key_scancode_t  scancode;
  key_flags_t     flags;
  UTIL_IF_1(USE_KEY_RAW_SCANCODE, uint32_t raw);
} key_t;

int key_to_char(key_t key);

key_t key_get();

key_t key_wait();

key_t key_get_port();

key_t key_wait_port();
