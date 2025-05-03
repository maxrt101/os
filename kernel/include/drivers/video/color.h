#pragma once

#include <stdint.h>

typedef enum {
  COLOR_ORDER_RGBA,
  COLOR_ORDER_ABGR,
  COLOR_ORDER_ARGB,
} color_order_t;

typedef struct {
  uint8_t r, g, b, a;
} color_t;

uint32_t video_color_pack(color_order_t order, color_t color);
