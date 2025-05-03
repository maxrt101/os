#pragma once

#include <drivers/video/framebuffer.h>
#include <time/timeout.h>
#include <util/lock.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  framebuffer_t * fb;

  font_t * font;

  color_t bg;
  color_t fg;

  position_t cursor;
  position_t saved_cursor;

  lock_t lock;

  struct {
    timeout_t timeout;
    bool state;
  } blink;
} tty_raw_t;

void tty_raw_init(tty_raw_t * tty, framebuffer_t * fb, font_t * font);
void tty_raw_print(tty_raw_t * tty, const char * buf);
char tty_raw_getch(tty_raw_t * tty);
void tty_raw_getline(tty_raw_t * tty, char * buf, size_t max);
bool tty_raw_getline_async(tty_raw_t * tty, char * buf, size_t max, size_t * index);
