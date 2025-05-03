#pragma once

#include <stdint.h>

typedef enum {
  FONT_TYPE_NONE = 0,
  FONT_TYPE_BITMAP_8_16
} font_type_t;

typedef struct {
  const uint8_t * data;

  font_type_t type;

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
