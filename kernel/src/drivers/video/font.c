#include <drivers/video/font.h>

uint32_t font_get_glyph_width(font_t * font) {
  switch (font->type) {
    case FONT_TYPE_BITMAP_8_16:
      return 8;

    default:
      break;
  }

  return 0;
}

uint32_t font_get_glyph_height(font_t * font) {
  switch (font->type) {
    case FONT_TYPE_BITMAP_8_16:
      return 16;

    default:
      break;
  }

  return 0;
}

glyph_t font_get_glyph(font_t * font, int codepoint) {
  if (codepoint < font->range.from || codepoint > font->range.to) {
    return (glyph_t){0};
  }

  switch (font->type) {
    case FONT_TYPE_BITMAP_8_16:
      return (glyph_t){font->data + (codepoint * 16)};

    default:
      break;
  }

  return (glyph_t){0};
}
