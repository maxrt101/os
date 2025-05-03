#include <drivers/tty/tty_raw.h>
#include <drivers/tty/ansi.h>
#include <drivers/keyboard/keyboard.h>
#include <util/assert.h>
#include <util/util.h>
#include <limits.h>

#define TAB_WIDTH_CHARS 8

__STATIC_INLINE void tty_raw_putc(tty_raw_t * tty, char ch);

__STATIC_INLINE void tty_raw_scroll(tty_raw_t * tty) {
  uint32_t pixels = font_get_glyph_height(tty->font);
  tty->cursor.y -= pixels;
  framebuffer_scroll(tty->fb, pixels, tty->bg);
}

__STATIC_INLINE void tty_raw_new_line(tty_raw_t * tty) {
  tty->cursor.x = 0;
  tty->cursor.y += font_get_glyph_height(tty->font);
  if (tty->cursor.y >= tty->fb->size.height) {
    tty_raw_scroll(tty);
  }
}

__STATIC_INLINE void tty_raw_backspace(tty_raw_t * tty) {
  if (tty->cursor.x > 0) {
    tty->cursor.x -= font_get_glyph_width(tty->font);
    tty_raw_putc(tty, ' ');
    tty->cursor.x -= font_get_glyph_width(tty->font);
  }
}

__STATIC_INLINE void tty_raw_putc(tty_raw_t * tty, char ch) {
  if (tty->cursor.y >= tty->fb->size.height) {
    tty->cursor.y = 0;
    tty->cursor.x = 0;
  }

  if (ch == '\t') {
    uint32_t cols = tty->cursor.x / font_get_glyph_width(tty->font);
    if (cols % TAB_WIDTH_CHARS != 0) {
      tty->cursor.x += (TAB_WIDTH_CHARS - (cols % TAB_WIDTH_CHARS)) * font_get_glyph_width(tty->font);
    }
    return;
  }

  if (ch == '\b') {
    tty_raw_backspace(tty);
    return;
  }

  if (ch == '\r') {
    tty->cursor.x = 0;
    return;
  }

  if (ch == '\n') {
    tty_raw_new_line(tty);
    return;
  }

  framebuffer_draw_char(
    tty->fb,
    tty->cursor,
    tty->font,
    ch,
    tty->fg,
    tty->bg
  );

  tty->cursor.x += font_get_glyph_width(tty->font);

  if (tty->cursor.x >= tty->fb->size.width) {
    tty_raw_new_line(tty);
  }
}

__STATIC_INLINE void tty_raw_set_ansi_color16(tty_raw_t * tty, ansi_color16_t color) {
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

void tty_raw_handle_ansi_esc_seq(ansi_esc_seq_t * seq, void * ctx) {
  tty_raw_t * tty = ctx;

  switch (seq->type) {
    case ANSI_ESC_SEQ_MOVE_CURSOR_HOME:
      tty->cursor.x = 0;
      tty->cursor.y = 0;
      break;

    case ANSI_ESC_SEQ_MOVE_CURSOR_TO:
      tty->cursor.y = seq->payload.move_cursor_to.line * font_get_glyph_height(tty->font);
      tty->cursor.x = seq->payload.move_cursor_to.col * font_get_glyph_width(tty->font);
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

      tty_raw_set_ansi_color16(tty, seq->payload.raw._1);
      tty_raw_set_ansi_color16(tty, seq->payload.raw._2);

      break;

    default:
      break;
  }
}

void tty_raw_init(tty_raw_t * tty, framebuffer_t * fb, font_t * font) {
  ASSERT_RETURN(tty && fb && font);

  tty->fb = fb;
  tty->font = font;
  tty->fg = (color_t){255, 255, 255};
  tty->bg = (color_t){0, 0, 0};
  tty->cursor.x = 0;
  tty->cursor.y = 0;
}

void tty_raw_print(tty_raw_t * tty, const char * buf) {
  ASSERT_RETURN(tty && buf);

  while (*buf) {
    if (*buf == '\033') {
      buf = ansi_parse_esc_seq(buf, tty_raw_handle_ansi_esc_seq, tty);
    } else {
      tty_raw_putc(tty, *buf++);
    }
  }
}

char tty_raw_getch(tty_raw_t * tty) {
  key_t key = key_wait();
  return key_to_char(key);
}

void tty_raw_getline(tty_raw_t * tty, char * buf, size_t max) {
  ASSERT_RETURN(tty && buf && max);

  size_t size = 0;

  while (size < max) {
    key_t key = key_wait();

    char c = key_to_char(key);

    if (key.scancode != KEY_SCANCODE_NONE && c != 0) {
      tty_raw_putc(tty, c);

      if (c == '\n') {
        break;
      }

      if (c == '\b') {
        tty_raw_putc(tty, '\b');
        size--;
        continue;
      }

      buf[size] = c;
      size++;
    }

  }

  buf[size] = '\0';
}

bool tty_raw_getline_async(tty_raw_t * tty, char * buf, size_t max, size_t * index) {
  ASSERT_RETURN(tty && buf && max && index, false);

  if (*index < max) {
    key_t key = key_get();

    char c = key_to_char(key);

    if (key.scancode != KEY_SCANCODE_NONE && c != 0) {
      if (c == '\n') {
        tty_raw_putc(tty, c);
        buf[*index] = '\0';
        return true;
      }

      if (c == '\b') {
        if (*index > 0) {
          tty_raw_putc(tty, '\b');
          (*index)--;
        }
        return false;
      }

      tty_raw_putc(tty, c);
      buf[*index] = c;
      (*index)++;
    }

    return false;
  }

  buf[*index] = '\0';
  return true;
}
