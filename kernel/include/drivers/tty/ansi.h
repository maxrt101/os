#pragma once

#define ANSI_ESC_CODE '\033'

typedef enum {
  ANSI_ESC_SEQ_INVALID = 0,
  ANSI_ESC_SEQ_MOVE_CURSOR_HOME,
  ANSI_ESC_SEQ_MOVE_CURSOR_TO,
  ANSI_ESC_SEQ_MOVE_CURSOR_UP,
  ANSI_ESC_SEQ_MOVE_CURSOR_DOWN,
  ANSI_ESC_SEQ_MOVE_CURSOR_RIGHT,
  ANSI_ESC_SEQ_MOVE_CURSOR_LEFT,
  ANSI_ESC_SEQ_MOVE_CURSOR_LINE_START_DOWN,
  ANSI_ESC_SEQ_MOVE_CURSOR_LINE_START_UP,
  ANSI_ESC_SEQ_MOVE_CURSOR_COLUMN,
  ANSI_ESC_SEQ_MOVE_CURSOR_LINE_UP,

  ANSI_ESC_SEQ_GET_CURSOR_POSITION,

  ANSI_ESC_SEQ_SAVE_CURSOR_POSITION,
  ANSI_ESC_SEQ_RESTORE_CURSOR_POSITION,

  ANSI_ESC_SEQ_ERASE_FROM_CURSOR_TO_END,
  ANSI_ESC_SEQ_ERASE_FROM_CURSOR_TO_START,
  ANSI_ESC_SEQ_ERASE_ENTIRE_SCREEN,
  ANSI_ESC_SEQ_ERASE_SAVED_LINES,
  ANSI_ESC_SEQ_ERASE_FROM_CURSOR_TO_LINE_END,
  ANSI_ESC_SEQ_ERASE_FROM_CURSOR_TO_LINE_START,
  ANSI_ESC_SEQ_ERASE_LINE,

  // TODO: Styles toggles
  ANSI_ESC_SEQ_SET_STYLE,

  // TODO: Other codes
} ansi_esc_seq_type_t;

typedef enum {
  ANSI_COLOR_FG_BLACK   = 30,
  ANSI_COLOR_FG_RED     = 31,
  ANSI_COLOR_FG_GREEN   = 32,
  ANSI_COLOR_FG_YELLOW  = 33,
  ANSI_COLOR_FG_BLUE    = 34,
  ANSI_COLOR_FG_MAGENTA = 35,
  ANSI_COLOR_FG_CYAN    = 36,
  ANSI_COLOR_FG_WHITE   = 37,
  ANSI_COLOR_FG_DEFAULT = 39,

  ANSI_COLOR_BG_BLACK   = 40,
  ANSI_COLOR_BG_RED     = 41,
  ANSI_COLOR_BG_GREEN   = 42,
  ANSI_COLOR_BG_YELLOW  = 43,
  ANSI_COLOR_BG_BLUE    = 44,
  ANSI_COLOR_BG_MAGENTA = 45,
  ANSI_COLOR_BG_CYAN    = 46,
  ANSI_COLOR_BG_WHITE   = 47,
  ANSI_COLOR_BG_DEFAULT = 49,
} ansi_color16_t;

typedef struct {
  ansi_esc_seq_type_t type;
  union {
    // TODO: Either add other payload aliases, or remove union altogether, leaving only _1, _2, ...
    struct { int _1, _2; } raw;
    struct { int line, col; } move_cursor_to;
  } payload;
} ansi_esc_seq_t;

typedef void (*ansi_esc_seq_handler_t)(ansi_esc_seq_t *, void *);

const char * ansi_parse_esc_seq(const char * str, ansi_esc_seq_handler_t handler, void * ctx);
