#pragma once

#include <stdint.h>
#include <drivers/video/primitives.h>
#include <drivers/video/color.h>
#include <drivers/video/font.h>

typedef struct {
  void * address;

  struct {
    uint64_t width;
    uint64_t height;
  } size;

  struct {
    uint64_t      bpp;
    uint64_t      pitch;
    uint64_t      width_bytes;
    color_order_t color_order;
  } info;
} framebuffer_t;

// void framebuffer_create(framebuffer_t * fb, uint64_t width, uint64_t height, uint8_t bpp);
// void framebuffer_destroy(framebuffer_t * fb);
// void framebuffer_clone(framebuffer_t * dest, framebuffer_t * src);

void framebuffer_copy(framebuffer_t * dest, framebuffer_t * src);
void framebuffer_blit(framebuffer_t * dest, framebuffer_t * src, position_t start, position_t size);
void framebuffer_scroll(framebuffer_t * fb, uint32_t n, color_t bg);

void framebuffer_draw_pixel(framebuffer_t * fb, position_t pos, color_t c);
void framebuffer_draw_line(framebuffer_t * fb, position_t start, position_t end, color_t c);
void framebuffer_draw_rect(framebuffer_t * fb, position_t start, position_t size, color_t c);
void framebuffer_draw_char(framebuffer_t * fb, position_t pos, font_t * font, int codepoint, color_t fg, color_t bg);
void framebuffer_draw_string(framebuffer_t * fb, position_t pos, font_t * font, const char * str, color_t fg, color_t bg);

