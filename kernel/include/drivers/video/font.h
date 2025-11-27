#pragma once

#include <stdbool.h>
#include <stdint.h>

// #define FONT_BITMAP_CREATE(__data, __x, __y, __from, __to, )

typedef enum {
  FONT_TYPE_NONE = 0,
  FONT_TYPE_BITMAP,
} font_type_t;

typedef struct {
  const uint8_t * data;

  font_type_t type;

  union {
    struct {
      struct {
        uint8_t x;
        uint8_t y;
      } size;
      bool reversed;
    } bitmap;
  } subtype;

  struct {
    uint16_t from;
    uint16_t to;
  } range;
} font_t;

typedef struct {
  // TODO: Union for different font types
  const uint8_t * start;
} glyph_t;

uint32_t font_get_glyph_width(font_t * font);

uint32_t font_get_glyph_height(font_t * font);

glyph_t font_get_glyph(font_t * font, int codepoint);
