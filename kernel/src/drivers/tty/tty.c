#include <drivers/tty/tty.h>
#include <drivers/tty/ansi.h>
#include <drivers/keyboard/keyboard.h>
#include <util/assert.h>
#include <kernel.h>
#include <limits.h>

#define TAB_WIDTH_CHARS 8

__STATIC_INLINE void tty_putc(tty_t * tty, char ch);

__STATIC_INLINE void tty_scroll(tty_t * tty) {
  uint32_t pixels = tty_get_glyph_height(tty);
  tty->cursor.y -= pixels;
  framebuffer_scroll(tty->fb, pixels, tty->bg);
}

__STATIC_INLINE void tty_new_line(tty_t * tty) {
  uint32_t height = tty_get_glyph_height(tty);

  tty->cursor.x = 0;
  tty->cursor.y += height;
  if (tty->cursor.y >= tty->fb->size.height - height) {
    tty_scroll(tty);
  }
}

__STATIC_INLINE void tty_backspace(tty_t * tty) {
  if (tty->cursor.x > 0) {
    tty->cursor.x -= tty_get_glyph_width(tty);
    tty_putc(tty, ' ');
    tty->cursor.x -= tty_get_glyph_width(tty);
  }
}

__STATIC_INLINE void tty_putc(tty_t * tty, char ch) {
  lock_waitlock(tty->lock);

  if (tty->cursor.y >= tty->fb->size.height) {
    tty->cursor.y = 0;
    tty->cursor.x = 0;
  }

  if (ch == '\t') {
    uint32_t cols = tty->cursor.x / tty_get_glyph_width(tty);
    if (cols % TAB_WIDTH_CHARS != 0) {
      tty->cursor.x += (TAB_WIDTH_CHARS - (cols % TAB_WIDTH_CHARS)) * tty_get_glyph_width(tty);
    }
    goto exit;
  }

  if (ch == '\b') {
    // FIXME: tty_backspace calls tty_putc, not releasing would result in deadlock
    //        good thing that double-unlock error is not a thing :)
    lock_release(tty->lock);
    tty_backspace(tty);
    goto exit;
  }

  if (ch == '\r') {
    tty->cursor.x = 0;
    goto exit;
  }

  if (ch == '\n') {
    tty_new_line(tty);
    goto exit;
  }

  framebuffer_draw_char(
    tty->fb,
    tty->cursor,
    tty->font,
    ch,
    tty->scale,
    tty->fg,
    tty->bg
  );

  tty->cursor.x += tty_get_glyph_width(tty);

  if (tty->cursor.x >= tty->fb->size.width) {
    tty_new_line(tty);
  }

exit:
  lock_release(tty->lock);
}

__STATIC_INLINE void tty_dcursor(tty_t * tty, bool state) {
  size_t char_width = tty_get_glyph_width(tty);
  size_t char_height = tty_get_glyph_height(tty);

  framebuffer_draw_rect(
    tty->fb,
    (position_t){tty->cursor.x + 1, tty->cursor.y + 1},
    (position_t){char_width - 1, char_height - 1},
    state ? tty->fg : tty->bg
  );
}

__STATIC_INLINE void tty_update_cursor(tty_t * tty) {
  if (timeout_is_expired(&tty->blink.timeout) && lock_trylock(tty->lock)) {
    tty_dcursor(tty, tty->blink.state);
    tty->blink.state = !tty->blink.state;

    timeout_restart(&tty->blink.timeout);

    lock_release(tty->lock);
  }
}

__STATIC_INLINE void tty_set_ansi_color16(tty_t * tty, ansi_color16_t color) {
  switch (color) {
    case ANSI_COLOR_FG_BLACK:   tty->fg = (color_t) {0, 0, 0}; break;
    case ANSI_COLOR_FG_RED:     tty->fg = (color_t) {255, 0, 0}; break;
    case ANSI_COLOR_FG_GREEN:   tty->fg = (color_t) {0, 255, 0}; break;
    case ANSI_COLOR_FG_YELLOW:  tty->fg = (color_t) {255, 255, 0}; break;
    case ANSI_COLOR_FG_BLUE:    tty->fg = (color_t) {0, 0, 255}; break;
    case ANSI_COLOR_FG_MAGENTA: tty->fg = (color_t) {255, 0, 255}; break;
    case ANSI_COLOR_FG_CYAN:    tty->fg = (color_t) {0, 255, 255}; break;
    case ANSI_COLOR_FG_WHITE:
    case ANSI_COLOR_FG_DEFAULT: tty->fg = (color_t) {255, 255, 255}; break;

    case ANSI_COLOR_BG_RED:     tty->bg = (color_t) {255, 0, 0}; break;
    case ANSI_COLOR_BG_GREEN:   tty->bg = (color_t) {0, 255, 0}; break;
    case ANSI_COLOR_BG_YELLOW:  tty->bg = (color_t) {255, 255, 0}; break;
    case ANSI_COLOR_BG_BLUE:    tty->bg = (color_t) {0, 0, 255}; break;
    case ANSI_COLOR_BG_MAGENTA: tty->bg = (color_t) {255, 0, 255}; break;
    case ANSI_COLOR_BG_CYAN:    tty->bg = (color_t) {0, 255, 255}; break;
    case ANSI_COLOR_BG_WHITE:   tty->bg = (color_t) {255, 255, 255}; break;
    case ANSI_COLOR_BG_BLACK:
    case ANSI_COLOR_BG_DEFAULT: tty->bg = (color_t) {0, 0, 0}; break;

    default:
      break;
  }
}

void tty_handle_ansi_esc_seq(ansi_esc_seq_t * seq, void * ctx) {
  tty_t * tty = ctx;

  switch (seq->type) {
    case ANSI_ESC_SEQ_MOVE_CURSOR_HOME:
      tty->cursor.x = 0;
      tty->cursor.y = 0;
      break;

    case ANSI_ESC_SEQ_MOVE_CURSOR_TO:
      tty->cursor.y = seq->payload.move_cursor_to.line * tty_get_glyph_height(tty);
      tty->cursor.x = seq->payload.move_cursor_to.col * tty_get_glyph_width(tty);
      break;

    case ANSI_ESC_SEQ_SAVE_CURSOR_POSITION:
      tty->saved_cursor.x = tty->cursor.x;
      tty->saved_cursor.y = tty->cursor.y;
      break;

    case ANSI_ESC_SEQ_RESTORE_CURSOR_POSITION:
      tty->cursor.x = tty->saved_cursor.x;
      tty->cursor.y = tty->saved_cursor.y;
      break;

    case ANSI_ESC_SEQ_ERASE_ENTIRE_SCREEN:
      tty->cursor.x = 0;
      tty->cursor.y = 0;
      framebuffer_draw_rect(
        tty->fb,
        tty->cursor,
        (position_t) {tty->fb->size.width, tty->fb->size.height},
        tty->bg
      );
      break;

    case ANSI_ESC_SEQ_SET_STYLE:
      if (seq->payload.raw._1 == 0 || seq->payload.raw._1 == INT_MAX) {
        tty->fg = (color_t) {255, 255, 255};
        tty->bg = (color_t) {0, 0, 0};
        break;
      }

      tty_set_ansi_color16(tty, seq->payload.raw._1);
      tty_set_ansi_color16(tty, seq->payload.raw._2);

      break;

    default:
      break;
  }
}

void tty_init(tty_t * tty, framebuffer_t * fb, font_t * font) {
  ASSERT_RETURN(tty && fb && font);

  tty->fb = fb;
  tty->font = font;
  tty->scale = INT2FIXED(1),
  tty->fg = (color_t){255, 255, 255};
  tty->bg = (color_t){0, 0, 0};
  tty->cursor.x = 0;
  tty->cursor.y = 0;
  tty->blink.state = false;

  lock_init(tty->lock);

  timeout_init(&tty->blink.timeout, time_milliseconds(500));
}

void tty_print(tty_t * tty, const char * buf) {
  ASSERT_RETURN(tty && buf);

  while (*buf) {
    if (*buf == '\033') {
      buf = ansi_parse_esc_seq(buf, tty_handle_ansi_esc_seq, tty);
    } else {
      tty_putc(tty, *buf++);
    }
  }
}

char tty_getch(tty_t * tty) {
  key_t key = key_wait();
  return key_to_char(key);
}

void tty_getline(tty_t * tty, char * buf, size_t max) {
  ASSERT_RETURN(tty && buf && max);

  size_t size = 0;

  while (size < max) {
    key_t key = key_get();

    char c = key_to_char(key);

    if (key.scancode != KEY_SCANCODE_NONE && c != 0) {
      tty_dcursor(tty, false);
      tty_putc(tty, c);

      if (c == '\n') {
        break;
      }

      if (c == '\b') {
        tty_putc(tty, '\b');
        size--;
        continue;
      }

      buf[size] = c;
      size++;
    }

    tty_update_cursor(tty);
  }

  buf[size] = '\0';
}

bool tty_getline_async(tty_t * tty, char * buf, size_t max, size_t * index) {
  ASSERT_RETURN(tty && buf && max && index, false);

  tty_update_cursor(tty);

  if (*index < max) {
    key_t key = key_get();

    char c = key_to_char(key);

    if (key.scancode != KEY_SCANCODE_NONE && c != 0) {
      tty_dcursor(tty, false);

      if (c == '\n') {
        tty_putc(tty, c);
        buf[*index] = '\0';
        return true;
      }

      if (c == '\b') {
        if (*index > 0) {
          tty_putc(tty, '\b');
          (*index)--;
        }
        return false;
      }

      tty_putc(tty, c);
      buf[*index] = c;
      (*index)++;
    }

    return false;
  }

  buf[*index] = '\0';
  return true;
}

uint32_t tty_get_glyph_width(tty_t * tty) {
  return FIXED_MUL_INT(font_get_glyph_width(tty->font), tty->scale);
}

uint32_t tty_get_glyph_height(tty_t * tty) {
  return FIXED_MUL_INT(font_get_glyph_height(tty->font), tty->scale);
}
