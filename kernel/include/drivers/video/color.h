#pragma once

#include <stdint.h>
#include <util/util.h>
#include <util/vargs.h>

#define COLOR(__r, __g, __b, ...) \
  (color_t){ .r = __r, .g = __g, .b = __b, .a = UTIL_IF_EMPTY(__VA_ARGS__, 255, __VA_ARGS__) }

typedef enum {
  COLOR_ORDER_RGBA,
  COLOR_ORDER_ABGR,
  COLOR_ORDER_ARGB,
} color_order_t;

typedef struct {
  uint8_t r, g, b, a;
} color_t;

uint32_t video_color_pack(color_order_t order, color_t color);
