#include <drivers/video/framebuffer.h>
#include <util/util.h>
#include <util/string.h>


#define FONT_BITMAP_GLYPH_SHIFT(__font, __x) \
  (__font->subtype.bitmap.reversed ? (7 - __x) : __x)


#define FONT_BITMAP_GET_PIXEL_COLOR(__font, __glyph, __y, __x, __fg, __bg)  \
  (                                                                         \
    (__glyph.start[__y] >> FONT_BITMAP_GLYPH_SHIFT(__font, __x)) & 1        \
      ? __fg                                                                \
      : __bg                                                                \
  )


void framebuffer_clone(framebuffer_t * dest, framebuffer_t * src) {
  memcpy(dest, src, sizeof(framebuffer_t));
}

void framebuffer_copy(framebuffer_t * dest, framebuffer_t * src) {
  memcpy(dest->address, src->address, src->info.width_bytes * src->size.height);
}

void framebuffer_blit(framebuffer_t * dest, framebuffer_t * src, position_t start, position_t size) {
  volatile uint32_t * dest_data = dest->address;
  volatile uint32_t * src_data = src->address;

  for (coordinate_t y = start.y; y < start.y + size.y; ++y) {
    for (coordinate_t x = start.x; x < start.x + size.x; ++x) {
      dest_data[y * (dest->info.pitch / 4) + x] = src_data[y * (src->info.pitch / 4) + x];
    }
  }
}

void framebuffer_scroll(framebuffer_t * fb, uint32_t n, color_t bg) {
  uint8_t * data = fb->address;
  uint8_t * from = (uint8_t*)fb->address + fb->info.width_bytes * n;

  for (uint32_t i = 0; i < fb->size.height - n; ++i) {
    memcpy(data + i * fb->info.width_bytes, from + i * fb->info.width_bytes, fb->info.width_bytes);
  }

  framebuffer_draw_rect(fb, (position_t){0, fb->size.height - n}, (position_t){fb->size.width, n}, bg);
}

void framebuffer_draw_pixel(framebuffer_t * fb, position_t pos, color_t c) {
  volatile uint32_t * data = fb->address;

  data[pos.y * (fb->info.pitch / 4) + pos.x] = video_color_pack(fb->info.color_order, c);
}

/* From wikipedia:
   dx = x2 − x1
   dy = y2 − y1
   m = dy/dx
   for x from x1 to x2 do
      y = m × (x − x1) + y1
      plot(x, y)
*/
void framebuffer_draw_line(framebuffer_t * fb, position_t start, position_t end, color_t c) {
  coordinate_t x1 = UTIL_MIN(start.x, end.x);
  coordinate_t x2 = UTIL_MAX(start.x, end.x);
  coordinate_t y1 = UTIL_MIN(start.y, end.y);
  coordinate_t y2 = UTIL_MAX(start.y, end.y);

  coordinate_t dx = x2 - x1;
  coordinate_t dy = y2 - y1;
  coordinate_t slope = dy/dx;

  for (coordinate_t x = x1; x < x2; ++x) {
    coordinate_t y = slope * (x - x1) + y1;
    framebuffer_draw_pixel(fb, (position_t){x, y}, c);
  }
}

void framebuffer_draw_rect(framebuffer_t * fb, position_t start, position_t size, color_t c) {
  for (coordinate_t y = start.y; y < start.y + size.y; ++y) {
    for (coordinate_t x = start.x; x < start.x + size.x; ++x) {
      framebuffer_draw_pixel(fb, (position_t){x, y}, c);
    }
  }
}

void framebuffer_draw_char(framebuffer_t * fb, position_t pos, font_t * font, int codepoint, fixed_t scale, color_t fg, color_t bg) {
  glyph_t glyph = font_get_glyph(font, codepoint);

  int scale_factor = FIXED_CEIL(scale);

  switch (font->type) {
    case FONT_TYPE_BITMAP: {
      for (coordinate_t y = 0; y < font->subtype.bitmap.size.y; ++y) {
        for (coordinate_t x = 0; x < font->subtype.bitmap.size.x; ++x) {
          framebuffer_draw_rect(
            fb,
            (position_t){pos.x + FIXED_MUL_INT(x, scale), pos.y + FIXED_MUL_INT(y, scale)},
            (position_t){scale_factor, scale_factor},
            FONT_BITMAP_GET_PIXEL_COLOR(font, glyph, y, x, fg, bg)
          );
        }
      }
      break;
    }

    default:
      break;
  }
}

void framebuffer_draw_string(framebuffer_t * fb, position_t pos, font_t * font, const char * str, fixed_t scale, color_t fg, color_t bg) {
  int dx = FIXED_MUL_INT(font_get_glyph_width(font), scale);

  while (*str) {
    framebuffer_draw_char(fb, pos, font, *str, scale, fg, bg);
    // pos.x += font_get_glyph_width(font) * scale;
    pos.x += dx;
    str++;
  }
}
