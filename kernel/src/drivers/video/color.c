#include <drivers/video/color.h>

uint32_t video_color_pack(color_order_t order, color_t color) {
  uint32_t result = 0;

  switch (order) {
    case COLOR_ORDER_RGBA:
      result |= color.r << 24;
      result |= color.g << 16;
      result |= color.b << 8;
      result |= color.a;
      break;

    case COLOR_ORDER_ABGR:
      result |= color.a << 24;
      result |= color.b << 16;
      result |= color.g << 8;
      result |= color.r;
      break;

    case COLOR_ORDER_ARGB:
      result |= color.a << 24;
      result |= color.r << 16;
      result |= color.g << 8;
      result |= color.b;
      break;

    default:
      break;
  }

  return result;
}
